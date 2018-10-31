#include "createcontractpage.h"
#include "ui_createcontractpage.h"
#include <qt/walletmodel.h>
#include <qt/clientmodel.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/abifunctionfield.h>

#include <qt/execrpccommand.h>
#include <qt/contractabi.h>

#include <qt/bitcoinunits.h>
#include <qt/sendcoinsdialog.h>
#include <qt/contractresult.h>

#include <QDebug>

namespace CreateContract_NS
{
// Contract data names
static const QString PRC_COMMAND = "createcontract";
static const QString PARAM_BYTECODE = "bytecode";
static const QString PARAM_GASLIMIT = "gaslimit";
static const QString PARAM_GASPRICE = "gasprice";
static const QString PARAM_SENDER = "sender";

static const CAmount SINGLE_STEP = 0.00000001*COIN;
static const CAmount HIGH_GASPRICE = 0.001*COIN;
}
using namespace CreateContract_NS;

CreateContractPage::CreateContractPage(QWidget *parent) : QWidget(parent),ui(new Ui::CreateContractPage)
{
    ui->setupUi(this);
    std::string platformName;
    platformStyle = PlatformStyle::instantiate(QString::fromStdString(platformName));
    if (!platformStyle)
        platformStyle = PlatformStyle::instantiate("other");

    qDebug()<<"platformStyle 1";
    assert(platformStyle);
    qDebug()<<"platformStyle 2";

    m_ABIFunctionField = new ABIFunctionField(platformStyle, ABIFunctionField::Create, ui->create_scrollArea);

    connect(ui->create_btn, SIGNAL(clicked()), SLOT(on_createContractClicked()));
    connect(ui->create_byteCodeText, SIGNAL(textChanged()), SLOT(on_updateCreateButton()));
    connect(ui->create_abiText, SIGNAL(textChanged()), SLOT(on_newContractABI()));

}


CreateContractPage::~CreateContractPage()
{
    delete ui;
}


void CreateContractPage::setModel(WalletModel *_model)
{
    m_model = _model;
}

QString CreateContractPage::toDataHex(int func, QString& errorMessage)
{
    if(func == -1 || m_ABIFunctionField == NULL || m_contractABI == NULL)
    {
        return "";
    }

    std::string strData;
    std::vector<std::vector<std::string>> values = m_ABIFunctionField->getValuesVector();
    FunctionABI function = m_contractABI->functions[func];
    std::vector<ParameterABI::ErrorType> errors;
    if(function.abiIn(values, strData, errors))
    {
        return QString::fromStdString(strData);
    }
    else
    {
        errorMessage = function.errorMessage(errors, true);
    }
    return "";
}


void CreateContractPage::on_createContractClicked()
{

    WalletModel::UnlockContext ctx(m_model->requestUnlock());
    if(!ctx.isValid())
    {
        return;
    }

    // Initialize variables
    QMap<QString, QString> lstParams;
    QVariant result;
    QString errorMessage;
    QString resultJson;
    int unit = m_model->getOptionsModel()->getDisplayUnit();

    uint64_t gasLimit = ui->create_gasLimitSpinBox->value();

    //FixMe: check if this convert has overflow problem?
    CAmount gasPrice = (int64_t)(ui->create_gasPriceEdit->text().toDouble()*COIN);


    int func = m_ABIFunctionField->getSelectedFunction();

    // Check for high gas price
    if(gasPrice > HIGH_GASPRICE)
    {
        QString message = tr("The Gas Price is too high, are you sure you want to possibly spend a max of %1 for this transaction?");
        if(QMessageBox::question(this, tr("High Gas price"), message.arg(BitcoinUnits::formatWithUnit(unit, gasLimit * gasPrice))) == QMessageBox::No)
            return;
    }

    // Append params to the list
    QString bytecode = ui->create_byteCodeText->toPlainText() + toDataHex(func, errorMessage);
    ExecRPCCommand::appendParam(lstParams, PARAM_BYTECODE, bytecode);
    ExecRPCCommand::appendParam(lstParams, PARAM_GASLIMIT, QString::number(gasLimit));
    ExecRPCCommand::appendParam(lstParams, PARAM_GASPRICE, BitcoinUnits::format(unit, gasPrice, false, BitcoinUnits::separatorNever));
    ExecRPCCommand::appendParam(lstParams, PARAM_SENDER, ui->create_senderAddress->text());

    QString questionString = tr("Are you sure you want to create contract? <br />");

    SendConfirmationDialog confirmationDialog(tr("Confirm contract creation."), questionString, 3, this);
    confirmationDialog.exec();
    QMessageBox::StandardButton retval = (QMessageBox::StandardButton)confirmationDialog.result();
    if(retval == QMessageBox::Yes)
    {
        // Execute RPC command line
        if(errorMessage.isEmpty() && m_execRPCCommand->exec(lstParams, result, resultJson, errorMessage))
        {
            ContractResult *widgetResult = new ContractResult(NULL);
            widgetResult->setResultData(result, FunctionABI(), QList<QStringList>(), ContractResult::CreateResult);
            //ui->stackedWidget->addWidget(widgetResult);
            int position = ui->tabview->count() - 1;
            m_results = position == 1 ? 1 : m_results + 1;

            ui->tabview->addTab(widgetResult,tr("Result %1").arg(m_results));
            ui->tabview->setCurrentIndex(position);
        }
        else
        {
            QMessageBox::warning(this, tr("Create contract"), errorMessage);
        }
    }

}
