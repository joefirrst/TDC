#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <string.h>
#include <stdio.h>
#include <tee_crypto_api.h>

#define KEY_SIZE 256  // ECDSA��Կ����
#define SIGNATURE_SIZE 64 // ǩ����С��������Բ����ѡ��

// ������Կ��
static TEE_Result generate_keypair(TEE_ObjectHandle* keypair) {
    TEE_Result res;
    TEE_Attribute attrs[2];

    // ������Կ����
    res = TEE_AllocateTransientObject(TEE_TYPE_ECDSA_KEYPAIR, KEY_SIZE, keypair);
    if (res != TEE_SUCCESS) {
        return res;
    }

    // ������Կ����
    TEE_InitRefAttribute(&attrs[0], TEE_ATTR_SECRET_VALUE, NULL, 0);
    TEE_InitValueAttribute(&attrs[1], TEE_ATTR_ECC_CURVE, TEE_ECC_CURVE_NIST_P256, 0);

    // ������Կ��
    return TEE_GenerateKey(*keypair, KEY_SIZE, attrs, 2);
}

// �����ݽ���ǩ��
static TEE_Result sign_data(TEE_ObjectHandle keypair, uint8_t* data, size_t data_len, uint8_t* signature, size_t* sig_len) {
    TEE_Result res;
    TEE_OperationHandle operation = TEE_HANDLE_NULL;

    // ����������
    res = TEE_AllocateOperation(&operation, TEE_ALG_ECDSA_SHA256, TEE_MODE_SIGN, 0);
    if (res != TEE_SUCCESS) {
        return res;
    }

    // ������Կ
    TEE_SetOperationKey(operation, keypair);

    // ǩ��
    res = TEE_AsymmetricSign(operation, NULL, 0, data, data_len, signature, sig_len);
    TEE_FreeOperation(operation);
    return res;
}

// ��֤ǩ��
static TEE_Result verify_signature(TEE_ObjectHandle pubkey, uint8_t* data, size_t data_len, uint8_t* signature, size_t sig_len) {
    TEE_Result res;
    TEE_OperationHandle operation = TEE_HANDLE_NULL;

    // ����������
    res = TEE_AllocateOperation(&operation, TEE_ALG_ECDSA_SHA256, TEE_MODE_VERIFY, 0);
    if (res != TEE_SUCCESS) {
        return res;
    }

    // ���ù�Կ
    TEE_SetOperationKey(operation, pubkey);

    // ��֤ǩ��
    res = TEE_AsymmetricVerify(operation, NULL, 0, data, data_len, signature, sig_len);
    TEE_FreeOperation(operation);
    return res;
}

// TA��������ں���
TEE_Result TA_InvokeCommandEntryPoint(void* sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    static TEE_ObjectHandle keypair = TEE_HANDLE_NULL;
    TEE_Result res;

    if (cmd_id == 0) {  // ������Կ��
        res = generate_keypair(&keypair);
        return res;
    }
    else if (cmd_id == 1) {  // ǩ������
        uint8_t* data = params[0].memref.buffer;
        size_t data_len = params[0].memref.size;
        uint8_t signature[SIGNATURE_SIZE];
        size_t sig_len = sizeof(signature);

        res = sign_data(keypair, data, data_len, signature, &sig_len);
        if (res == TEE_SUCCESS) {
            memcpy(params[1].memref.buffer, signature, sig_len);
            params[1].memref.size = sig_len;
        }
        return res;
    }
    else if (cmd_id == 2) {  // ��֤ǩ��
        uint8_t* data = params[0].memref.buffer;
        size_t data_len = params[0].memref.size;
        uint8_t* signature = params[1].memref.buffer;
        size_t sig_len = params[1].memref.size;

        // ����Ӧ����һ��������ȡ��Կ���˴������ж�Ӧ��pubkey
        TEE_ObjectHandle pubkey = keypair; // �滻Ϊʵ�ʹ�Կ����

        return verify_signature(pubkey, data, data_len, signature, sig_len);
    }

    return TEE_ERROR_BAD_PARAMETERS;
}

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("Signature TA Created\n");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
    DMSG("Signature TA Destroyed\n");
}
