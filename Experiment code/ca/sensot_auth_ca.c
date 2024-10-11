#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>
#include "sensor_auth_ta.h"

#define SENSOR_ID "sensor1234567890"  // 示例传感器ID

int main() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_Result res;
    TEEC_UUID uuid = TA_SENSOR_AUTH_UUID;

    // 初始化 TEE 环境
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("Failed to initialize context\n");
        return -1;
    }

    // 打开 TEE 会话
    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to open session\n");
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // 记录传感器
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = SENSOR_ID;
    op.params[0].tmpref.size = strlen(SENSOR_ID);

    res = TEEC_InvokeCommand(&sess, 0, &op, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to record sensor\n");
    }

    // 更新传感器信息（模拟通信次数增加）
    res = TEEC_InvokeCommand(&sess, 1, &op, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to update sensor\n");
    }

    // 断开传感器
    res = TEEC_InvokeCommand(&sess, 2, &op, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to disconnect sensor\n");
    }

    // 输出日志
    res = TEEC_InvokeCommand(&sess, 3, &op, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to output log\n");
    }

    // 关闭会话和上下文
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}
