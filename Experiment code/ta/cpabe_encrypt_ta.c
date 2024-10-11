#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <pbc.h>  // 引入PBC库
#include <string.h>

#define AES_KEY_SIZE 16 // AES-128 密钥长度 (16 bytes)
#define AES_BLOCK_SIZE 16 // AES 数据块大小 (16 bytes)
#define CPABE_KEY_SIZE 256 // 假设 CP-ABE 生成的密钥长度

// 加密命令 ID
#define TA_AES_ENCRYPT_CMD 0
#define TA_CPABE_ENCRYPT_CMD 1

static TEE_ObjectHandle aes_key = NULL;
static pairing_t pairing;
static element_t public_key, master_key;

// 生成 AES-128 密钥的函数
static TEE_Result generate_aes_key(uint8_t* key, size_t key_len) {
    TEE_GenerateRandom(key, key_len);
    return TEE_SUCCESS;
}

// AES加密函数
static TEE_Result aes_encrypt(uint8_t* in_data, uint32_t in_len, uint8_t* out_data, uint32_t* out_len, uint8_t* key) {
    TEE_Result res;
    TEE_OperationHandle op = NULL;

    res = TEE_AllocateOperation(&op, TEE_ALG_AES_ECB_NOPAD, TEE_MODE_ENCRYPT, AES_KEY_SIZE * 8);
    if (res != TEE_SUCCESS) {
        return res;
    }

    TEE_SetOperationKeyWith2Constants(op, TEE_TYPE_AES, TEE_HANDLE_FLAG_PERSISTENT, key, AES_KEY_SIZE);

    // 执行 AES 加密操作
    res = TEE_CipherUpdate(op, in_data, in_len, out_data, out_len);
    TEE_FreeOperation(op);
    return res;
}

// 初始化CP-ABE参数
static TEE_Result init_cpabe_params() {
    // 配置 pairing 参数和属性
    char param[1024];
    snprintf(param, sizeof(param), "type a\nq 87807107996633125224377819847540498158068831994142082"
        "1102865339926647563088022295707862517942266222142315585\nh 120160122648911460793888213667405342048029544012513118229759875312499"
        "548815027019983474166041971207032221804233878353787593\nr 730750818665451621361119245571504901405976559617\nexp2 159\nexp1 107\nsign1 1\n"
        "sign0 -1");

    pairing_init_set_buf(pairing, param, strlen(param));
    element_init_G1(public_key, pairing);
    element_init_Zr(master_key, pairing);

    // 假设公共密钥和主密钥是预先生成好的
    element_random(public_key);
    element_random(master_key);

    return TEE_SUCCESS;
}

// CP-ABE加密 AES 密钥
static TEE_Result cpabe_encrypt(uint8_t* aes_key, uint32_t aes_key_len, uint8_t* cpabe_key_out, uint32_t* cpabe_key_len) {
    element_t message, cipher;

    // 初始化
    element_init_Zr(message, pairing);
    element_init_G1(cipher, pairing);

    // 将AES密钥转换为CP-ABE加密
    element_from_hash(message, aes_key, aes_key_len);
    element_pow_zn(cipher, public_key, message);

    // 导出加密结果到输出缓冲区
    memcpy(cpabe_key_out, cipher, element_length_in_bytes(cipher));
    *cpabe_key_len = element_length_in_bytes(cipher);

    // 清理
    element_clear(message);
    element_clear(cipher);

    return TEE_SUCCESS;
}

// TA入口点，处理AES加密和CP-ABE加密命令
TEE_Result TA_InvokeCommandEntryPoint(void* sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    switch (cmd_id) {
    case TA_AES_ENCRYPT_CMD:
    {
        uint8_t aes_key[AES_KEY_SIZE];
        uint32_t out_len = params[1].memref.size;

        // 生成 AES 密钥并加密文件
        generate_aes_key(aes_key, AES_KEY_SIZE);
        return aes_encrypt(params[0].memref.buffer, params[0].memref.size, params[1].memref.buffer, &out_len, aes_key);
    }
    case TA_CPABE_ENCRYPT_CMD:
    {
        uint8_t cpabe_key[CPABE_KEY_SIZE];
        uint32_t cpabe_key_len = CPABE_KEY_SIZE;

        // 使用 CP-ABE 加密 AES 密钥
        return cpabe_encrypt(params[0].memref.buffer, AES_KEY_SIZE, cpabe_key, &cpabe_key_len);
    }
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}

TEE_Result TA_CreateEntryPoint(void) {
    return init_cpabe_params();
}

void TA_DestroyEntryPoint(void) {
    element_clear(public_key);
    element_clear(master_key);
}
