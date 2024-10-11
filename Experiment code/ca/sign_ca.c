#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>
#include "signature_ta.h"

#define DATA "Hello, TEE!" // 示例数据

int main() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_Result res;
    TEEC_UUID uuid = TA_SIGNATURE_UUID;

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

    // 生成密钥对
    res = TEEC_InvokeCommand(&sess, 0, NULL, NULL);
    if (res != TEEC_SUCCESS) {
        printf("Failed to generate keypair\n");
    }

    // 签名数据
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = DATA;
    op.params[0].tmpref.size = strlen(DATA);
    uint8_t signature[SIGNATURE_SIZE];
    op.params[1].tmpref.buffer = signature;
    op.params[1].tmpref.size = sizeof(signature);

    res = TEEC_InvokeCommand(&sess, 1, &op, NULL);
    if (res == TEEC_SUCCESS) {
        printf("Signature created successfully\n");
    }
    else {
        printf("Failed to sign data\n");
    }

    // 验证签名
    op.params[0].tmpref.buffer = DATA;
    op.params[0].tmpref.size = strlen(DATA);
    op.params[1].tmpref.buffer = signature;
    op.params[1].tmpref.size = op.params[1].tmpref.size; // 之前获得的签名大小

    res = TEEC_InvokeCommand(&sess, 2, &op, NULL);
    if (res == TEEC_SUCCESS) {
        printf("Signature verified successfully\n");
    }
    else {
        printf("Failed to verify signature\n");
    }

    // 关闭会话和上下文
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}
