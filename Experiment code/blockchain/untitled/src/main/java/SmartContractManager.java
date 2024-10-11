import org.web3j.crypto.Credentials;
import org.web3j.protocol.Web3j;
import org.web3j.protocol.http.HttpService;



/**
 * ClassName: SmartContractManager
 * Package: PACKAGE_NAME
 * Description:
 *
 * @Author:
 * @Create: 2024/10/11 - 16:16
 * @Version: v1.0
 */


public class SmartContractManager {
    private Web3j web3j;
    private Credentials credentials;

    public SmartContractManager(String privateKey, String nodeUrl) {
        this.web3j = Web3j.build(new HttpService(nodeUrl)); // 连接到区块链节点
        this.credentials = Credentials.create(privateKey); // 使用私钥进行签名
    }

    public Web3j getWeb3j() {
        return web3j;
    }

    public Credentials getCredentials() {
        return credentials;
    }
}

