#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <string.h>

#define AES_KEY_SIZE 16 // AES-128 ��Կ���� (16 bytes)
#define AES_BLOCK_SIZE 16 // AES ���ݿ��С (16 bytes)

// �������� ID
#define TA_AES_ENCRYPT_CMD 0

// �洢��Կ�����ȫ�ֱ���
static TEE_ObjectHandle aes_key = NULL;

// ���� AES-128 ��Կ�ĺ���
static TEE_Result generate_aes_key(void) {
    TEE_Result res;
    TEE_Attribute attr;

    // ���� AES ��Կ
    res = TEE_AllocateTransientObject(TEE_TYPE_AES, AES_KEY_SIZE * 8, &aes_key);
    if (res != TEE_SUCCESS) {
        return res;
    }

    // ���������Կ
    TEE_GenerateKey(aes_key, AES_KEY_SIZE * 8, NULL, 0);
    return TEE_SUCCESS;
}

// ���ܺ���
static TEE_Result aes_encrypt(uint8_t* in_data, uint32_t in_len, uint8_t* out_data, uint32_t* out_len) {
    TEE_Result res;
    TEE_OperationHandle op = NULL;

    // ���� AES-128 ECB ���ܲ���
    res = TEE_AllocateOperation(&op, TEE_ALG_AES_ECB_NOPAD, TEE_MODE_ENCRYPT, AES_KEY_SIZE * 8);
    if (res != TEE_SUCCESS) {
        return res;
    }

    res = TEE_SetOperationKey(op, aes_key);
    if (res != TEE_SUCCESS) {
        TEE_FreeOperation(op);
        return res;
    }

    // ִ�� AES ���ܲ���
    res = TEE_CipherUpdate(op, in_data, in_len, out_data, out_len);
    if (res != TEE_SUCCESS) {
        TEE_FreeOperation(op);
        return res;
    }

    TEE_FreeOperation(op);
    return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void* sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    if (cmd_id == TA_AES_ENCRYPT_CMD) {
        uint32_t expected_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
            TEE_PARAM_TYPE_MEMREF_OUTPUT,
            TEE_PARAM_TYPE_NONE,
            TEE_PARAM_TYPE_NONE);

        if (param_types != expected_param_types) {
            return TEE_ERROR_BAD_PARAMETERS;
        }

        // ���� AES ���ܺ���
        return aes_encrypt(params[0].memref.buffer, params[0].memref.size, params[1].memref.buffer, &params[1].memref.size);
    }

    return TEE_ERROR_BAD_PARAMETERS;
}

TEE_Result TA_CreateEntryPoint(void) {
    return generate_aes_key();
}

void TA_DestroyEntryPoint(void) {
    if (aes_key) {
        TEE_FreeTransientObject(aes_key);
    }
}
