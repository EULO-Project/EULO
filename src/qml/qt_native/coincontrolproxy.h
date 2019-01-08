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
    Q_INVOKABLE void updateView();
    Q_INVOKABLE QVariant getData(QString roleName, int sourceRow);
    Q_INVOKABLE void setData_(int sourceRow, QString value, int mode);


};

#endif // COINCONTROLPROXY_H
