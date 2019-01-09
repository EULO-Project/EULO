#ifndef COINCONTROLPROXY_H
#define COINCONTROLPROXY_H

#include "coincontrolmodel.h"
#include <QSortFilterProxyModel>

class CoinControlProxy : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    explicit CoinControlProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);
    Q_INVOKABLE void updateView(QVariantList payAmountList);
    Q_INVOKABLE QVariant getData(QString roleName, int sourceRow);
    Q_INVOKABLE void setData_(int sourceRow, QString value, int mode);
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void updateSendingPage();
    Q_INVOKABLE void updateSplitUtxo(bool checked, const QString &utxo, const QString &afterFee);
    Q_INVOKABLE QString updatecustomChangeAddress(bool checked,const QString &address);
    Q_INVOKABLE void setValue(int index, QVariant value, QVariantList payAmountList);
    Q_INVOKABLE QVariant getValue(int index);



signals:
    void updateLabels(QVariantList msg);

    void showCoinControl(bool show);
    void updateLabelBlockSize(QString size);



};

#endif // COINCONTROLPROXY_H
