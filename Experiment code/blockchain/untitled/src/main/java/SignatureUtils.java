import java.security.KeyFactory;
import java.security.PublicKey;
import java.security.spec.X509EncodedKeySpec;
import java.util.Base64;

/**
 * ClassName: SignatureUtils
 * Package: PACKAGE_NAME
 * Description:
 *
 * @Author:
 * @Create: 2024/10/11 - 16:18
 * @Version: v1.0
 */
public class SignatureUtils {
    public static boolean verifySignature(String data, String publicKeyStr, String signatureStr) throws Exception {
        PublicKey publicKey = getPublicKeyFromBase64(publicKeyStr);

        Signature signature = Signature.getInstance("SHA256withRSA");
        signature.initVerify(publicKey);
        signature.update(data.getBytes());

        byte[] signatureBytes = Base64.getDecoder().decode(signatureStr);
        return signature.verify(signatureBytes);
    }

    private static PublicKey getPublicKeyFromBase64(String key) throws Exception {
        byte[] decodedKey = Base64.getDecoder().decode(key);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        return keyFactory.generatePublic(new X509EncodedKeySpec(decodedKey));
    }
}
