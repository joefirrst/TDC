/**
 * ClassName: DataProcessingManager
 * Package: PACKAGE_NAME
 * Description:
 *
 * @Author:
 * @Create: 2024/10/11 - 16:18
 * @Version: v1.0
 */
public class DataProcessingManager {
    private SmartContractManager contractManager;
    private DataContract dataContract;

    public DataProcessingManager(SmartContractManager contractManager, DataContract dataContract) {
        this.contractManager = contractManager;
        this.dataContract = dataContract;
    }

    public void processAndStoreData(String encryptedData, String aesKey, String signature, String publicKey) throws Exception {
        // Step 1: 解密数据
        String decryptedData = AESUtils.decrypt(encryptedData, aesKey);

        // Step 2: 验证签名
        boolean isSignatureValid = SignatureUtils.verifySignature(decryptedData, publicKey, signature);
        if (!isSignatureValid) {
            throw new SecurityException("Signature validation failed.");
        }

        // Step 3: 调用智能合约存储数据
        String key = "DataKey";
        dataContract.storeData(key, decryptedData).send();
        System.out.println("Data stored successfully.");
    }
}
