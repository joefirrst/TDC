#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <string.h>
#include <stdio.h>

#define SENSOR_ID_LENGTH 16   // 假设传感器ID为16字节
#define UUID_LENGTH 32        // UUID的长度
#define MAX_LOG_ENTRIES 100   // 最大日志条目数量

typedef struct {
    char uuid[UUID_LENGTH];
    uint32_t start_time;    // 记录连接时的时间戳
    uint32_t run_time;      // 运行时间
    uint32_t comm_count;    // 通信次数
    uint32_t disconnect_time; // 最后断开的时间戳
} sensor_info_t;

// 传感器的日志数组
static sensor_info_t sensor_log[MAX_LOG_ENTRIES];
static int log_index = 0;  // 日志条目计数器

// 获取当前时间戳
static uint32_t get_current_timestamp(void) {
    TEE_Time time;
    TEE_GetSystemTime(&time);
    return time.seconds;
}

// 生成 UUID 的函数
static void generate_sensor_uuid(const char* sensor_id, uint32_t timestamp, char* uuid_out) {
    char buffer[SENSOR_ID_LENGTH + sizeof(uint32_t)];
    memcpy(buffer, sensor_id, SENSOR_ID_LENGTH);
    memcpy(buffer + SENSOR_ID_LENGTH, &timestamp, sizeof(uint32_t));

    // 使用 SHA256 来生成 UUID
    TEE_SHA256Digest(buffer, sizeof(buffer), uuid_out, UUID_LENGTH);
}

// 记录传感器信息
static TEE_Result record_sensor_info(const char* sensor_id) {
    if (log_index >= MAX_LOG_ENTRIES) {
        return TEE_ERROR_OUT_OF_MEMORY;
    }

    sensor_info_t* entry = &sensor_log[log_index++];
    uint32_t timestamp = get_current_timestamp();

    // 生成并存储UUID
    generate_sensor_uuid(sensor_id, timestamp, entry->uuid);

    // 初始化传感器信息
    entry->start_time = timestamp;
    entry->run_time = 0;
    entry->comm_count = 0;
    entry->disconnect_time = 0;

    return TEE_SUCCESS;
}

// 更新传感器的通信次数和运行时间
static TEE_Result update_sensor_info(const char* uuid) {
    for (int i = 0; i < log_index; i++) {
        if (strncmp(sensor_log[i].uuid, uuid, UUID_LENGTH) == 0) {
            sensor_log[i].run_time = get_current_timestamp() - sensor_log[i].start_time;
            sensor_log[i].comm_count++;
            return TEE_SUCCESS;
        }
    }
    return TEE_ERROR_ITEM_NOT_FOUND;
}

// 断开传感器并记录断开时间
static TEE_Result disconnect_sensor(const char* uuid) {
    for (int i = 0; i < log_index; i++) {
        if (strncmp(sensor_log[i].uuid, uuid, UUID_LENGTH) == 0) {
            sensor_log[i].disconnect_time = get_current_timestamp();
            return TEE_SUCCESS;
        }
    }
    return TEE_ERROR_ITEM_NOT_FOUND;
}

// 打印日志信息
static void output_log(void) {
    for (int i = 0; i < log_index; i++) {
        sensor_info_t* entry = &sensor_log[i];
        DMSG("UUID: %s, Start Time: %u, Run Time: %u seconds, Communication Count: %u, Disconnect Time: %u",
            entry->uuid, entry->start_time, entry->run_time, entry->comm_count, entry->disconnect_time);
    }
}

// TA的命令入口函数
TEE_Result TA_InvokeCommandEntryPoint(void* sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    if (cmd_id == 0) {  // 记录传感器
        if (param_types != TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE)) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        return record_sensor_info((char*)params[0].memref.buffer);
    }
    else if (cmd_id == 1) {  // 更新传感器信息
        if (param_types != TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE)) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        return update_sensor_info((char*)params[0].memref.buffer);
    }
    else if (cmd_id == 2) {  // 断开传感器
        if (param_types != TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE)) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        return disconnect_sensor((char*)params[0].memref.buffer);
    }
    else if (cmd_id == 3) {  // 输出日志
        output_log();
        return TEE_SUCCESS;
    }

    return TEE_ERROR_BAD_PARAMETERS;
}

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("Sensor Authentication TA Created\n");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
    DMSG("Sensor Authentication TA Destroyed\n");
}
