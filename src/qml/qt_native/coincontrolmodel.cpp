#include "coincontrolmodel.h"
#include "addresstablemodel.h"
#include "bitcoinunits.h"
#include "guiutil.h"
#include "init.h"
#include "optionsmodel.h"
#include "walletmodel.h"

#include "coincontrol.h"
#include "obfuscation.h"
#include "wallet.h"

#include <QDebug>


CoinControlModel::CoinControlModel(CWallet* wallet, WalletModel* parent, bool MultisigEnabled) :
    QAbstractListModel(parent),
    model(parent),
    fMultisigEnabled(MultisigEnabled)
{

}

int CoinControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    qDebug()<<"vecOuts.size called:"<<vecOuts.size();

    return vecOuts.size();
}

int CoinControlModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

QModelIndex CoinControlModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(vecOuts.size() <= row)
        return QModelIndex();

    return createIndex(row, column, nullptr);

}


bool CoinControlModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    QString status = value.toString();
    COutput  out =  vecOuts.at(index.row()).first;
    COutPoint outpt(out.tx->GetHash(), out.i);

    if (role == CoinControlModel::StatusRole)
    {
        if(status == "1")
        {
            model->lockCoin(outpt);
            mapSelection[QString::fromStdString(out.ToString())] = false;
        }
        else
        {
            model->unlockCoin(outpt);
        }

    }
    else if (role == CoinControlModel::SelectionRole)
    {
        if(status == "1")
        {
            mapSelection[QString::fromStdString(out.ToString())] = true;
        }
        else
        {
            mapSelection[QString::fromStdString(out.ToString())] = false;
        }

        qDebug()<<"changed";
    }




    emit dataChanged(index, index);
}

QVariant CoinControlModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QString sWalletAddress = vecOuts.at(index.row()).second;
    COutput  out =  vecOuts.at(index.row()).first;
    isminetype mine = pwalletMain->IsMine(out.tx->vout[out.i]);
    bool fMultiSigUTXO = (mine & ISMINE_MULTISIG);
    int nInputSize = 0;

    int nDisplayUnit = model->getOptionsModel()->getDisplayUnit();
    double mempoolEstimatePriority = mempool.estimatePriority(nTxConfirmTarget);


    CTxDestination outputAddress;
    QString sAddress = "";
    if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, outputAddress)) {
        sAddress = QString::fromStdString(CBitcoinAddress(outputAddress).ToString());

        CPubKey pubkey;
        CKeyID* keyid = boost::get<CKeyID>(&outputAddress);
        if (keyid && model->getPubKey(*keyid, pubkey) && !pubkey.IsCompressed())
            nInputSize = 29; // 29 = 180 - 151 (public key is 180 bytes, priority free area is 151 bytes)
    }

    double dPriority = ((double)out.tx->vout[out.i].nValue / (nInputSize + 78)) * (out.nDepth + 1);

    QString sLabel;
    if (!(sAddress == sWalletAddress))
    {
        sLabel = tr("(change)");
    }
    else
    {
        sLabel = model->getAddressTableModel()->labelForAddress(sAddress);
        if (sLabel.isEmpty())
            sLabel = tr("(no label)");
    }

    bool selected;


    if(mapSelection.find(QString::fromStdString(out.ToString())) != mapSelection.end())
        selected = mapSelection.at(QString::fromStdString(out.ToString()));
    else
        selected = false;

    bool locked =  model->isLockedCoin(out.tx->GetHash(), out.i);
    QString confirmationStr = strPad(QString::number(out.nDepth), 8, QString(" "));

    QString statusStr;

    if(selected)
        statusStr = "1";
    else
        statusStr = "0";

    if(locked)
        statusStr += "|1";
    else
        statusStr += "|0";



    switch (role) {

    case Qt::EditRole:
        // Edit role is used for sorting, so return the unformatted values
        switch (index.column()) {
        case Amount:
            return (qint64)(out.tx->vout[out.i].nValue);
        case Date:
            return (qint64)(out.tx->GetTxTime());
        case Type:
            return fMultiSigUTXO ? "MultiSig" : "Personal";
        case Label:
            return sLabel;
        case Address:
            return sAddress;
        case Confirmations:
            return QString::number(out.nDepth);
        case Priority:
            return dPriority;

        }
        break;

    case AmountRole:
        return BitcoinUnits::format(nDisplayUnit, out.tx->vout[out.i].nValue);
    case TxHashRole:
        return QString::fromStdString(out.tx->GetHash().GetHex());
    case LabelRole:
        return sLabel;
    case AddressRole:
        return sAddress;
    case TypeRole:
        return fMultiSigUTXO ? "MultiSig" : "Personal";
    case DateRole:
        return GUIUtil::dateTimeStr(out.tx->GetTxTime());
    case ConfirmedRole:
        return confirmationStr;
    case PriorityRole:
        return CoinControlModel::getPriorityLabel(dPriority, mempoolEstimatePriority);
    case StatusRole:
        return statusStr;

    }
    return QVariant();

}

QString CoinControlModel::getPriorityLabel(double dPriority, double mempoolEstimatePriority)
{
    double dPriorityMedium = mempoolEstimatePriority;

    if (dPriorityMedium <= 0)
        dPriorityMedium = AllowFreeThreshold(); // not enough data, back to hard-coded

    if (dPriority / 1000000 > dPriorityMedium)
        return tr("highest");
    else if (dPriority / 100000 > dPriorityMedium)
        return tr("higher");
    else if (dPriority / 10000 > dPriorityMedium)
        return tr("high");
    else if (dPriority / 1000 > dPriorityMedium)
        return tr("medium-high");
    else if (dPriority > dPriorityMedium)
        return tr("medium");
    else if (dPriority * 10 > dPriorityMedium)
        return tr("low-medium");
    else if (dPriority * 100 > dPriorityMedium)
        return tr("low");
    else if (dPriority * 1000 > dPriorityMedium)
        return tr("lower");
    else
        return tr("lowest");
}

QString CoinControlModel::strPad(QString s, int nPadLength, QString sPadding) const
{
    while (s.length() < nPadLength)
        s = sPadding + s;

    return s;
}

QHash<int, QByteArray> CoinControlModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[StatusRole] = "status";
    roles[AmountRole] = "amount";
    roles[LabelRole] = "label";
    roles[AddressRole] = "address";
    roles[TypeRole] = "type";
    roles[DateRole] = "date";
    roles[ConfirmedRole] = "confirmations";
    roles[PriorityRole] = "priority";
    roles[TxHashRole] = "txid";



    return roles;
}

void CoinControlModel::updateView()
{
    beginRemoveRows(QModelIndex(), 0, vecOuts.size() - 1);
    mapCoins.clear();
    vecOuts.clear();
    model->listCoins(mapCoins);
    endRemoveRows();

    qDebug()<<"mapCoins.size:"<<mapCoins.size();
    qDebug()<<"fMultisigEnabled:"<<fMultisigEnabled;

    BOOST_FOREACH (PAIRTYPE(QString, vector<COutput>) coins, mapCoins)
    {

        QString sWalletAddress = coins.first;


        for(const COutput& out: coins.second)
        {
            isminetype mine = pwalletMain->IsMine(out.tx->vout[out.i]);
            bool fMultiSigUTXO = (mine & ISMINE_MULTISIG);
            // when multisig is enabled, it will only display outputs from multisig addresses
            qDebug()<<"mine:"<<mine;
            qDebug()<<"fMultiSigUTXO:"<<fMultiSigUTXO;

            if (fMultisigEnabled && !fMultiSigUTXO)
                continue;

            vecOuts.push_back(make_pair(out,sWalletAddress));
        }

    }
    beginInsertRows(QModelIndex(), 0, vecOuts.size() - 1);

    endInsertRows();

    qDebug()<<"vecOuts.size:"<<vecOuts.size();
}

