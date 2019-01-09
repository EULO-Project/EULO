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
#include "utilmoneystr.h"

#include <QDebug>
#include <QSettings>


QList<CAmount> CoinControlModel::payAmounts;
int CoinControlModel::nSplitBlockDummy;
CCoinControl* CoinControlModel::coinControl = new CCoinControl();

CoinControlModel::CoinControlModel(CWallet* wallet, WalletModel* parent, bool MultisigEnabled) :
    QAbstractListModel(parent),
    model(parent),
    fMultisigEnabled(MultisigEnabled)
{
    QSettings settings;
    if (!settings.contains("bUseObfuScation"))
        settings.setValue("bUseObfuScation", false);
    if (!settings.contains("bUseSwiftTX"))
        settings.setValue("bUseSwiftTX", false);

    if (!settings.contains("fFeeSectionMinimized"))
        settings.setValue("fFeeSectionMinimized", true);
    if (!settings.contains("nFeeRadio") && settings.contains("nTransactionFee") && settings.value("nTransactionFee").toLongLong() > 0) // compatibility
        settings.setValue("nFeeRadio", true);                                                                                             // custom
    if (!settings.contains("nFeeRadio"))
        settings.setValue("nFeeRadio", false);                                                                                                   // recommended
    if (!settings.contains("nCustomFeeRadio") && settings.contains("nTransactionFee") && settings.value("nTransactionFee").toLongLong() > 0) // compatibility
        settings.setValue("nCustomFeeRadio", true);                                                                                             // total at least
    if (!settings.contains("nCustomFeeRadio"))
        settings.setValue("nCustomFeeRadio", false); // per kilobyte
    if (!settings.contains("nSmartFeeSliderPosition"))
        settings.setValue("nSmartFeeSliderPosition", 0);
    if (!settings.contains("nTransactionFee"))
        settings.setValue("nTransactionFee", (qint64)DEFAULT_TRANSACTION_FEE);
    if (!settings.contains("fPayOnlyMinFee"))
        settings.setValue("fPayOnlyMinFee", false);
    if (!settings.contains("fSendFreeTransactions"))
        settings.setValue("fSendFreeTransactions", false);


}

QVariant CoinControlModel::getValue(int index)
{
    QSettings settings;
    switch(index)
    {
    case 0:
        return settings.value("fFeeSectionMinimized").toBool();
    case 1:
        return settings.value("nFeeRadio").toBool();
    case 2:
        return settings.value("nCustomFeeRadio").toBool();
    case 3:
        return settings.value("nSmartFeeSliderPosition").toBool();
    case 4:
        return settings.value("nTransactionFee").toLongLong();
    case 5:
        return settings.value("fPayOnlyMinFee").toBool();
    case 6:
        return  settings.value("fSendFreeTransactions").toBool();
    }
}


void CoinControlModel::setValue(int index, QVariant value, QVariantList payAmountList)
{
    QSettings settings;
    switch(index)
    {
    case 0:
        settings.setValue("fFeeSectionMinimized", value.toBool());
        break;
    case 1:
        settings.setValue("nFeeRadio", value.toBool());
        break;
    case 2:
        settings.setValue("nCustomFeeRadio", value.toBool());
        break;
    case 3:
        settings.setValue("nSmartFeeSliderPosition", value.toInt());
        break;
    case 4:
        settings.setValue("nTransactionFee", value.toLongLong());
        break;
    case 5:
        settings.setValue("fPayOnlyMinFee", value.toBool());
        break;
    case 6:
        settings.setValue("fSendFreeTransactions", value.toBool());
        break;
    }


    CoinControlModel::payAmounts.clear();

    for(int i = 0;i<payAmountList.size();i++)
        CoinControlModel::payAmounts.append(payAmountList[i].toLongLong());

    if (CoinControlModel::coinControl->HasSelected())
    {
        // actual coin control calculation
        CoinControlModel::updateLabelsFunc(model,mapSelection,this);
    }

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

void CoinControlModel::updateLabelsFunc(WalletModel* model, std::map<QString,bool> &mapSelection,CoinControlModel *obj)
{
    if (!model)
        return;

    // nPayAmount
    CAmount nPayAmount = 0;
    bool fDust = false;
    CMutableTransaction txDummy;
    foreach (const CAmount& amount, CoinControlModel::payAmounts) {
        nPayAmount += amount;

        if (amount > 0) {
            CTxOut txout(amount, (CScript)vector<unsigned char>(24, 0));
            txDummy.vout.push_back(txout);
            if (txout.IsDust(::minRelayTxFee))
                fDust = true;
        }
    }

    QString sPriorityLabel = tr("none");
    CAmount nAmount = 0;
    CAmount nPayFee = 0;
    CAmount nAfterFee = 0;
    CAmount nChange = 0;
    unsigned int nBytes = 0;
    unsigned int nBytesInputs = 0;
    double dPriority = 0;
    double dPriorityInputs = 0;
    unsigned int nQuantity = 0;
    int nQuantityUncompressed = 0;
    bool fAllowFree = false;

    vector<COutPoint> vCoinControl;
    vector<COutput> vOutputs;
    coinControl->ListSelected(vCoinControl);
    model->getOutputs(vCoinControl, vOutputs);

    BOOST_FOREACH (const COutput& out, vOutputs) {
        // unselect already spent, very unlikely scenario, this could happen
        // when selected are spent elsewhere, like rpc or another computer
        uint256 txhash = out.tx->GetHash();
        COutPoint outpt(txhash, out.i);
        if (model->isSpent(outpt)) {
            coinControl->UnSelect(outpt);
            mapSelection[QString::fromStdString(out.ToString())] = false;
            continue;
        }

        // Quantity
        nQuantity++;

        // Amount
        nAmount += out.tx->vout[out.i].nValue;

        // Priority
        dPriorityInputs += (double)out.tx->vout[out.i].nValue * (out.nDepth + 1);

        // Bytes
        CTxDestination address;
        if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, address)) {
            CPubKey pubkey;
            CKeyID* keyid = boost::get<CKeyID>(&address);
            if (keyid && model->getPubKey(*keyid, pubkey)) {
                nBytesInputs += (pubkey.IsCompressed() ? 148 : 180);
                if (!pubkey.IsCompressed())
                    nQuantityUncompressed++;
            } else
                nBytesInputs += 148; // in all error cases, simply assume 148 here
        } else
            nBytesInputs += 148;
    }

    // calculation
    if (nQuantity > 0) {
        // Bytes
        nBytes = nBytesInputs + ((CoinControlModel::payAmounts.size() > 0 ? CoinControlModel::payAmounts.size() + max(1, CoinControlModel::nSplitBlockDummy) : 2) * 34) + 10; // always assume +1 output for change here

        // Priority
        double mempoolEstimatePriority = mempool.estimatePriority(nTxConfirmTarget);
        dPriority = dPriorityInputs / (nBytes - nBytesInputs + (nQuantityUncompressed * 29)); // 29 = 180 - 151 (uncompressed public keys are over the limit. max 151 bytes of the input are ignored for priority)
        sPriorityLabel = CoinControlModel::getPriorityLabel(dPriority, mempoolEstimatePriority);

        // Fee
        nPayFee = CWallet::GetMinimumFee(nBytes, nTxConfirmTarget, mempool);

        // IX Fee
        if (coinControl->useSwiftTX) nPayFee = max(nPayFee, CENT);
        // Allow free?
        double dPriorityNeeded = mempoolEstimatePriority;
        if (dPriorityNeeded <= 0)
            dPriorityNeeded = AllowFreeThreshold(); // not enough data, back to hard-coded
        fAllowFree = (dPriority >= dPriorityNeeded);

        if (fSendFreeTransactions)
            if (fAllowFree && nBytes <= MAX_FREE_TRANSACTION_CREATE_SIZE)
                nPayFee = 0;

        if (nPayAmount > 0) {
            nChange = nAmount - nPayFee - nPayAmount;

            // Never create dust outputs; if we would, just add the dust to the fee.
            if (nChange > 0 && nChange < CENT) {
                CTxOut txout(nChange, (CScript)vector<unsigned char>(24, 0));
                if (txout.IsDust(::minRelayTxFee)) {
                    nPayFee += nChange;
                    nChange = 0;
                }
            }

            if (nChange == 0)
                nBytes -= 34;
        }

        // after fee
        nAfterFee = nAmount - nPayFee;
        if (nAfterFee < 0)
            nAfterFee = 0;
    }

    // actually update labels
    int nDisplayUnit = BitcoinUnits::ULO;
    if (model && model->getOptionsModel())
        nDisplayUnit = model->getOptionsModel()->getDisplayUnit();

    QVariantList returnList;
    bool needReverse = nPayFee > 0 && !(payTxFee.GetFeePerK() > 0 && fPayAtLeastCustomFee && nBytes < 1000);
    // stats
    returnList.append(QString::number(nQuantity));// Quantity
    returnList.append(BitcoinUnits::formatWithUnit(nDisplayUnit, nAmount));// Amount
    returnList.append(needReverse?("~" + BitcoinUnits::formatWithUnit(nDisplayUnit, nPayFee)):BitcoinUnits::formatWithUnit(nDisplayUnit, nPayFee));// Fee
    returnList.append(needReverse?("~" + BitcoinUnits::formatWithUnit(nDisplayUnit, nAfterFee)):BitcoinUnits::formatWithUnit(nDisplayUnit, nAfterFee));// After Fee
    returnList.append(((nBytes > 0) ? "~" : "") + QString::number(nBytes));// Bytes
    returnList.append(sPriorityLabel);// Priority
    returnList.append(fDust ? tr("yes") : tr("no"));// Dust
    returnList.append((needReverse && (nChange > 0))?("~" + BitcoinUnits::formatWithUnit(nDisplayUnit, nChange)):BitcoinUnits::formatWithUnit(nDisplayUnit, nChange));// Change


    obj->callUpdateLabels(returnList);

}

void CoinControlModel::callUpdateLabels(QVariantList returnList)
{
    emit updateLabels(returnList);
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
            coinControl->UnSelect(outpt);
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
            coinControl->Select(outpt);

        }
        else
        {
            mapSelection[QString::fromStdString(out.ToString())] = false;
            coinControl->UnSelect(outpt);
        }

    }




    emit dataChanged(index, index);

    CoinControlModel::updateLabelsFunc(model,mapSelection,this);

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


void CoinControlModel::selectAll()
{
    bool hasSelected = false;

    for(int row = 0 ;row< vecOuts.size();row++)
    {
        COutput  out =  vecOuts.at(row).first;

        if(mapSelection[QString::fromStdString(out.ToString())])
        {
            hasSelected = true;
            break;
        }

    }

    for(int row = 0 ;row< vecOuts.size();row++){

        COutput  out =  vecOuts.at(row).first;
        COutPoint outpt(out.tx->GetHash(), out.i);

        bool locked =  model->isLockedCoin(out.tx->GetHash(), out.i);

        if(!locked)
        {
            if(hasSelected)
            {
                mapSelection[QString::fromStdString(out.ToString())] = false;
                coinControl->UnSelect(outpt);
            }
            else
            {
                mapSelection[QString::fromStdString(out.ToString())] = true;
                coinControl->Select(outpt);

            }

        }


    }


    emit dataChanged(index(0,0), index(vecOuts.size()-1,0));

    CoinControlModel::updateLabelsFunc(model,mapSelection,this);
}

void CoinControlModel::toggle()
{
    for(int row = 0 ;row< vecOuts.size();row++){

        COutput  out =  vecOuts.at(row).first;
        COutPoint outpt(out.tx->GetHash(), out.i);
        bool locked =  model->isLockedCoin(out.tx->GetHash(), out.i);

        if(locked)
        {
            model->unlockCoin(outpt);
        }
        else
        {
            model->lockCoin(outpt);
            mapSelection[QString::fromStdString(out.ToString())] = false;
            coinControl->UnSelect(outpt);

        }

    }

    emit dataChanged(index(0,0), index(vecOuts.size()-1,0));
    CoinControlModel::updateLabelsFunc(model,mapSelection,this);

}

void CoinControlModel::updateView(QVariantList payAmountList)
{
    beginRemoveRows(QModelIndex(), 0, vecOuts.size() - 1);
    mapCoins.clear();
    vecOuts.clear();
    model->listCoins(mapCoins);
    endRemoveRows();

    BOOST_FOREACH (PAIRTYPE(QString, vector<COutput>) coins, mapCoins)
    {

        QString sWalletAddress = coins.first;


        for(const COutput& out: coins.second)
        {
            isminetype mine = pwalletMain->IsMine(out.tx->vout[out.i]);
            bool fMultiSigUTXO = (mine & ISMINE_MULTISIG);
            // when multisig is enabled, it will only display outputs from multisig addresses            if (fMultisigEnabled && !fMultiSigUTXO)
            if (fMultisigEnabled && !fMultiSigUTXO)
                continue;

            vecOuts.push_back(make_pair(out,sWalletAddress));
        }

    }
    beginInsertRows(QModelIndex(), 0, vecOuts.size() - 1);

    endInsertRows();

    CoinControlModel::payAmounts.clear();

    for(int i = 0;i<payAmountList.size();i++)
        CoinControlModel::payAmounts.append(payAmountList[i].toLongLong());

    if (CoinControlModel::coinControl->HasSelected())
    {
        // actual coin control calculation
        CoinControlModel::updateLabelsFunc(model,mapSelection,this);
    }

}

void CoinControlModel::updateSplitUtxo(bool checked,const QString &utxo,const QString &afterFee)
{

    CoinControlModel::coinControl->fSplitBlock = checked;
    fSplitBlock = checked;

    if(!checked)
    {
        return;
    }

    QString qAfterFee = afterFee.left(afterFee.indexOf(" ")).replace("~", "").simplified().replace(" ", "");

    //convert to CAmount
    CAmount nAfterFee;
    ParseMoney(qAfterFee.toStdString().c_str(), nAfterFee);

    //if greater than 0 then divide after fee by the amount of blocks
    CAmount nSize = nAfterFee;
    int nBlocks = utxo.toInt();
    if (nAfterFee && nBlocks)
        nSize = nAfterFee / nBlocks;

    //assign to split block dummy, which is used to recalculate the fee amount more outputs
    CoinControlModel::nSplitBlockDummy = nBlocks;

    //update labels
    emit updateLabelBlockSize(QString::fromStdString(FormatMoney(nSize)));


}


QString CoinControlModel::updatecustomChangeAddress(bool checked,const QString &address)
{
    CoinControlModel::coinControl->destChange = CNoDestination();

    if(!checked) return "";

    CBitcoinAddress addr = CBitcoinAddress(address.toStdString());

    if (address.isEmpty()) // Nothing entered
    {
        return "empty";
    } else if (!addr.IsValid()) // Invalid address
    {
        return "Invalid address";
    } else // Valid address
    {
        CPubKey pubkey;
        CKeyID keyid;
        addr.GetKeyID(keyid);
        if (!model->getPubKey(keyid, pubkey)) // Unknown change address
        {
            return "Unknown change address";
        } else // Known change address
        {

            // Query label
            QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
            CoinControlModel::coinControl->destChange = addr.Get();

            if (!associatedLabel.isEmpty())
                return associatedLabel;
            else
                return tr("(no label)");


        }
    }

}

