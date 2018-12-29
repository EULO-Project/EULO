#ifndef CREATECONTRACTPAGE_H
#define CREATECONTRACTPAGE_H

#include <QObject>
#include <QMessageBox>
#include <QVariantList>

class PlatformStyle;
class WalletModel;
class ClientModel;
class ExecRPCCommand;
class ABIFunctionField;
class ContractABI;
class ParameterABI;
class FunctionABI;

class CreateContractPage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList createFunctionList MEMBER m_createFunctionList NOTIFY createFunctionListChanged)
    Q_PROPERTY(QStringList sendtoFunctionList MEMBER m_sendtoFunctionList NOTIFY sendtoFunctionListChanged)
    Q_PROPERTY(QStringList callFunctionList MEMBER m_callFunctionList NOTIFY callFunctionListChanged)


public:
    explicit CreateContractPage(WalletModel* walletModel = 0);
    ~CreateContractPage();

    enum ContractMode
    {
        Create = 0,
        SendTo = 1,
        Call = 2
    };

    Q_INVOKABLE bool updateContractABI(int mode, const QString &abiStr);
    Q_INVOKABLE QVariantList functionChanged(int mode, int currentIndex);
    Q_INVOKABLE bool isFunctionPayable(int index);

    Q_INVOKABLE QString getLastResult();

    Q_INVOKABLE quint64 getDefaultGasLimitOpCreate();

    Q_INVOKABLE quint64 getDefaultGasLimitOpSend();


    Q_INVOKABLE void createContractClicked(QString gasLimitStr,
                                            int gasPriceUint,
                                            QString gasPriceStr,
                                            QString byteCode,
                                            QString senderAddress,
                                            QVariantList paramList);


    Q_INVOKABLE void sendToContractClicked(int functionIndex,
                                            QString gasLimitStr,
                                            int gasPriceUint,
                                            QString gasPriceStr,
                                            QString contractAddress,
                                            QString amountStr,
                                            int amountUint,
                                            QString senderAddress,
                                            QVariantList paramList);




    Q_INVOKABLE void callContractClicked(int functionIndex,
                                            QString contractAddress,
                                            QString senderAddress,
                                            QVariantList paramList);



signals:
    void createFunctionListChanged();
    void sendtoFunctionListChanged();
    void callFunctionListChanged();

    void notifyContractResult(QString title,bool error,QString errMsg,int type,QString resultStr);


public slots:


private:
    WalletModel* m_model;
    ClientModel* m_clientModel;
    //ContractTableModel* m_contractModel;
    QString m_lastResult = "";

    enum ContractTxType{
        CreateResult,
        SendToResult,
        CallResult
    };


    //--------Create------------
    ExecRPCCommand* m_execRPCCommand;
    ABIFunctionField* m_ABIFunctionField;
    ContractABI* m_createContractABI;
    QStringList m_createFunctionList;
    QVector<int> m_createFunctionArray;

    int m_results;
    //--------Create------------

    //--------Sendto------------
    ExecRPCCommand* m_execRPCCommand_sendto;
    ABIFunctionField* m_ABIFunctionField_sendto;
    ContractABI* m_sendtoContractABI;
    int m_results_sendto;
    QStringList m_sendtoFunctionList;
    QVector<int> m_sendtoFunctionArray;
    //--------Sendto------------


    //---------Call-------------
    ExecRPCCommand* m_execRPCCommand_call;
    ABIFunctionField* m_ABIFunctionField_call;
    ContractABI* m_callContractABI;
    int m_results_call;
    QStringList m_callFunctionList;
    QVector<int> m_callFunctionArray;

    //---------Call-------------

    const PlatformStyle* platformStyle;


    QString toDataHex(int func, QString& errorMessage, QVariantList &paramList);
    QString toDataHex_Sendto(int func, QString& errorMessage,QVariantList &paramList);
    QString toDataHex_Call(int func, QString& errorMessage, QVariantList &paramList);

    bool isValidContractAddress();




    void RenewContractFunctions(ContractABI *abi, ContractMode mode, QStringList *functionList, QVector<int> *functionArray);
    std::tuple<std::string, std::string, bool,int, std::string> ParseParams(const ParameterABI &param);

    std::vector<std::vector<std::string>> getValuesVector(QVariantList &paramList);
    QString setParamsData(FunctionABI &function, QVariantList paramValues);

private slots:
    //--------Create------------
    void on_updateCreateButton();
    //--------Create------------

    //--------Sendto------------
    void on_updateSendToContractButton();
    //void on_newContractABI_sendto();
    void on_functionChanged();
    void on_contractAddressChanged();
    //--------Sendto------------

    //---------Call-------------
    void on_updateCallContractButton();
    void on_newContractABI_call();
    //---------Call-------------


};

#endif // CREATECONTRACTPAGE_H
