#ifndef COINCONTROLMODEL_H
#define COINCONTROLMODEL_H
#include "bitcoinunits.h"
#include "wallet.h"

#include <QAbstractTableModel>
#include <QStringList>

class TransactionRecord;
class TransactionTablePriv;
class WalletModel;
class ClientModel;
class CCoinControl;

class CWallet;

class CoinControlModel : public QAbstractListModel
{
    Q_OBJECT


public:
   explicit CoinControlModel(CWallet *wallet, WalletModel *parent = 0,bool MultisigEnabled = false);


    enum ColumnIndex {
        Status = 0,
        Date = 1,
        Type = 2,
        Address = 3,
        Label = 4,
        Confirmations = 5,
        Priority = 6,
        Amount = 7
    };

    enum RoleIndex {
        /** Type of transaction */
        TypeRole = Qt::UserRole,
        DateRole,
        AddressRole,
        LabelRole,
        AmountRole,
        StatusRole,
        SelectionRole,
        ConfirmedRole,
        PriorityRole,
        TxHashRole,
        };



    bool fSplitBlock;

    void updateView(QVariantList payAmountList);
    void toggle();
    void selectAll();
    void updateSplitUtxo(bool checked,const QString &utxo, const QString &afterFee);
    QString updatecustomChangeAddress(bool checked, const QString &address);
    void setValue(int index, QVariant value, QVariantList payAmountList);

    QVariant getValue(int index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role);
    void callUpdateLabels(QVariantList returnList);

    static QList<CAmount> payAmounts;
    static CCoinControl* coinControl;
    static int nSplitBlockDummy;

    static QString getPriorityLabel(double dPriority, double mempoolEstimatePriority);
    static void updateLabelsFunc(WalletModel* model, std::map<QString, bool> &mapSelection, CoinControlModel *obj);

protected:
    QHash<int, QByteArray> roleNames() const ;



private:
    WalletModel* model;
    bool fMultisigEnabled;

    std::map<QString, std::vector<COutput>> mapCoins;
    std::vector<std::pair<COutput,QString>> vecOuts;

    std::map<QString,bool> mapSelection;

    QString strPad(QString s, int nPadLength, QString sPadding) const;

signals:
    void updateLabels(QVariantList msg);
    void updateLabelBlockSize(QString size);
};

#endif // COINCONTROLMODEL_H
