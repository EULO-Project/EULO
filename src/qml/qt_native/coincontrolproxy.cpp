#include "coincontrolproxy.h"

CoinControlProxy::CoinControlProxy(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}


void CoinControlProxy::updateView()
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->updateView();
}

void CoinControlProxy::setData_(int sourceRow, QString value,int mode)
{
    QModelIndex index_ = mapToSource(this->index(sourceRow,0));
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());


    if(mode == 0)
        sourceModel_->setData(index_, value, CoinControlModel::StatusRole);
    else
        sourceModel_->setData(index_, value, CoinControlModel::SelectionRole);



}


QVariant CoinControlProxy::getData(QString roleName, int sourceRow)
{
    if(roleName == "txid"){
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::TxHashRole).toString();
    }
    else if(roleName == "label")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::LabelRole).toString();
    }
    else if(roleName == "amount")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::AmountRole).toString();
    }

    else if(roleName == "address")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::AddressRole).toString();
    }
}


void CoinControlProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    CoinControlModel::ColumnIndex ci;

    if(roleName == "type")
        ci = CoinControlModel::Type;
    else if(roleName == "date")
        ci = CoinControlModel::Date;
    else if(roleName == "address")
        ci = CoinControlModel::Address;
    else if(roleName == "amount")
        ci = CoinControlModel::Amount;
    else if(roleName == "status")
        return;
    else if(roleName == "priority")
        ci = CoinControlModel::Priority;
    else if(roleName == "label")
        ci = CoinControlModel::Label;
    else if(roleName == "confirmations")
        ci = CoinControlModel::Confirmations;
    sort(ci,order);
}
