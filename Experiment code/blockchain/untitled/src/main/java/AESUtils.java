import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.util.Base64;

/**
 * ClassName: AESUtils
 * Package: PACKAGE_NAME
 * Description:
 *
 * @Author:
 * @Create: 2024/10/11 - 16:17
 * @Version: v1.0
 */
public class AESUtils {
    public static String decrypt(String encryptedData, String key) throws Exception {
        byte[] decodedKey = Base64.getDecoder().decode(key);
        SecretKeySpec secretKey = new SecretKeySpec(decodedKey, "AES");

        Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
        cipher.init(Cipher.DECRYPT_MODE, secretKey);

        byte[] decodedData = Base64.getDecoder().decode(encryptedData);
        byte[] decryptedData = cipher.doFinal(decodedData);

        return new String(decryptedData);
    }
}
