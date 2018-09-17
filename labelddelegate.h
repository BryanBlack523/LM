#ifndef LABELDDELEGATE_H
#define LABELDDELEGATE_H
#include <QItemDelegate>

class LabelDDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit LabelDDelegate(QObject *parent = 0);
protected:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // LABELDDELEGATE_H
