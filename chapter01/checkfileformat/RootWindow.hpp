#ifndef __ROOTWINDOW_HPP
#define __ROOTWINDOW_HPP

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qabstractitemmodel.h>

class RootWindow : public QAbstractItemModel {
    Q_OBJECT
public:
    RootWindow(QObject * parent = nullptr);
public slots:
    void checkTheDir(QVariant);
public:
    Q_SIGNAL void finishedCheckTheDir();
private:
    using Super = QAbstractItemModel;
    QVector<QString> _m_add_bom;
    QVector<QString> _m_remove_bom;
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
};

#endif // ROOTWINDOW_HPP
