#ifndef CREATECONTRACTPAGE_H
#define CREATECONTRACTPAGE_H

#include <QWidget>
#include <QMessageBox>

class PlatformStyle;
class WalletModel;
class ClientModel;
class ExecRPCCommand;
class ABIFunctionField;
class ContractABI;


namespace Ui
{
class CreateContractPage;
}

class CreateContractPage : public QWidget
{
    Q_OBJECT
public:
    explicit CreateContractPage(QWidget *parent = nullptr);
    ~CreateContractPage();

    void setModel(WalletModel *_model);

signals:

public slots:


private:
    Ui::CreateContractPage* ui;
    WalletModel* m_model;
    ClientModel* m_clientModel;
    ExecRPCCommand* m_execRPCCommand;
    ABIFunctionField* m_ABIFunctionField;
    ContractABI* m_contractABI;
    int m_results;

    const PlatformStyle* platformStyle;


    QString toDataHex(int func, QString& errorMessage);

private slots:
    void on_createContractClicked();


};

#endif // CREATECONTRACTPAGE_H
