#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>
#include "cpabe_encrypt_ta.h"

#define AES_KEY_SIZE 16
#define CPABE_KEY_SIZE 256

int main(int argc, char* argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_CPABE_ENCRYPT_UUID;

    // 初始化 TEE 环境
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("Failed to initialize TEE context\n");
        return -1;
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to open TEE session\n");
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // 调用 TA 执行 AES 加密和 CP-ABE 加密
    uint8_t aes_key[AES_KEY_SIZE];
    uint8_t cpabe_key[CPABE_KEY_SIZE];
    uint8_t file_data[256];  // 假设要加密的文件数据
    uint32_t cpabe_key_len = CPABE_KEY_SIZE;

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

    // 执行 AES 加密
    op.params[0].tmpref.buffer = file_data;
    op.params[0].tmpref.size = sizeof(file_data);
    op.params[1].tmpref.buffer = aes_key;
    op.params[1].tmpref.size = sizeof(aes_key);
    res = TEEC_InvokeCommand(&sess, TA_AES_ENCRYPT_CMD, &op, NULL);

    // 执行 CP-ABE 加密
    op.params[0].tmpref.buffer = aes_key;
    op.params[0].tmpref.size = AES_KEY_SIZE;
    op.params[1].tmpref.buffer = cpabe_key;
    op.params[1].tmpref.size = cpabe_key_len;
    res = TEEC_InvokeCommand(&sess, TA_CPABE_ENCRYPT_CMD, &op, NULL);

    if (res == TEEC_SUCCESS) {
        printf("AES 密钥使用 CP-ABE 成功加密\n");
    }

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
