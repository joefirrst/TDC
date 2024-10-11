import org.web3j.tx.gas.DefaultGasProvider;

/**
 * ClassName: SmartContractDeployer
 * Package: PACKAGE_NAME
 * Description:
 *
 * @Author:
 * @Create: 2024/10/11 - 16:17
 * @Version: v1.0
 */
public class SmartContractDeployer {
    private SmartContractManager contractManager;

    public SmartContractDeployer(SmartContractManager contractManager) {
        this.contractManager = contractManager;
    }

    public DataContract deployContract() throws Exception {
        return DataContract.deploy(
                contractManager.getWeb3j(),
                contractManager.getCredentials(),
                new DefaultGasProvider()
        ).send();
    }
}
