#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "aes_encrypt_ta.h"

#define AES_BLOCK_SIZE 16 // AES ���ݿ��С

// ��ȡ�ļ����ݵ�������
int read_file(const char* filename, uint8_t** buffer, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = malloc(*size);
    if (!*buffer) {
        fclose(file);
        return -1;
    }

    fread(*buffer, 1, *size, file);
    fclose(file);
    return 0;
}

// д��������ݵ��ļ�
int write_file(const char* filename, const uint8_t* buffer, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return -1;
    }

    fwrite(buffer, 1, size, file);
    fclose(file);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return -1;
    }

    // ��ʼ�� TEE �ͻ���
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_Result res;
    TEEC_UUID uuid = TA_AES_ENCRYPT_UUID;

    // �� TEE �����Ự
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed\n");
        return -1;
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, NULL);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed\n");
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // ��ȡ�ļ�����
    uint8_t* file_data = NULL;
    size_t file_size = 0;
    if (read_file(argv[1], &file_data, &file_size) != 0) {
        printf("Failed to read file\n");
        TEEC_CloseSession(&sess);
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // Ϊ��������������ڴ�
    uint8_t* encrypted_data = malloc(file_size);
    if (!encrypted_data) {
        free(file_data);
        TEEC_CloseSession(&sess);
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // ���� TA ���� AES ����
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = file_data;
    op.params[0].tmpref.size = file_size;
    op.params[1].tmpref.buffer = encrypted_data;
    op.params[1].tmpref.size = file_size;

    res = TEEC_InvokeCommand(&sess, TA_AES_ENCRYPT_CMD, &op, NULL);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed\n");
        free(file_data);
        free(encrypted_data);
        TEEC_CloseSession(&sess);
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // ����������д���ļ�
    if (write_file(argv[2], encrypted_data, file_size) != 0) {
        printf("Failed to write file\n");
    }

    // ������Դ
    free(file_data);
    free(encrypted_data);
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    printf("File encrypted successfully\n");
    return 0;
}
