#include "activity.h"


Activity::Activity(const QString &name, const QModelIndex &tableIndex, const QModelIndex &listIndex)
{
    m_name = name;
    m_tableIdx = tableIndex;
    m_listIdx = listIndex;
}

//Activity::Activity(){}

Activity::~Activity()
{

}

QModelIndex Activity::getListIndex()
{
    return m_listIdx;
}

QModelIndex Activity::getTableIndex()
{
    return m_tableIdx;
}

QString Activity::getName()
{
    return m_name;
}
