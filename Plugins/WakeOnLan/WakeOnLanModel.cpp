#include <QLoggingCategory>
#include <QIcon>
#include <QNetworkInterface>

#include "WakeOnLanModel.h"
#include "ParameterWakeOnLanUI.h"

static Q_LOGGING_CATEGORY(log, "WakeOnLan.Model")
CWakeOnLanModel::CWakeOnLanModel(QObject *parent)
    : QAbstractTableModel{parent}
    , m_Colume(6)
{}

CWakeOnLanModel::~CWakeOnLanModel()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CWakeOnLanModel::rowCount(const QModelIndex &parent) const
{
    return m_Data.size();
}

int CWakeOnLanModel::columnCount(const QModelIndex &parent) const
{
    return m_Colume;
}

QVariant CWakeOnLanModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant val;
    if(Qt::Horizontal != orientation)
        return val;
    if(Qt::DisplayRole == role)
    {
        if(0 == section)
            val = tr("State");
        else if(1 == section)
            val = tr("IP");
        else if(2 == section)
            val = tr("MAC");
        else if(3 == section)
            val = tr("Broadcast address");
        else if(4 == section)
            val = tr("Network interface");
        else if(5 == section)
            val = tr("Port");
        return val;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags CWakeOnLanModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    int r = index.row();
    int c = index.column();
    if(r >= m_Data.size() || c >= m_Colume)
        return Qt::NoItemFlags;
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if(1 == c || 2 == c) {
        return Qt::ItemIsEditable | f;
    }
    return f;
}

QVariant CWakeOnLanModel::data(const QModelIndex &index, int role) const
{
    QVariant val;
    int r = index.row();
    int c = index.column();
    if(r >= m_Data.size() || c >= m_Colume)
        return val;

    auto para = m_Data.at(r);

    if(Qt::BackgroundRole == role)
        switch(para->GetHostState()) {
        case CParameterWakeOnLan::HostState::Online:
            val = QBrush(Qt::green);
            break;
        case CParameterWakeOnLan::HostState::GetMac:
            val = QBrush(Qt::yellow);
            break;
        case CParameterWakeOnLan::HostState::WakeOnLan:
            val = QBrush(Qt::blue);
            break;
        default:
            break;
        }

    if(0 == c) {
        switch(para->GetHostState()) {
        case CParameterWakeOnLan::HostState::Online:
            if(Qt::DecorationRole == role)
                val = QIcon::fromTheme("network-online");
            if(Qt::DisplayRole == role)
                val = tr("Online");
            break;
        case CParameterWakeOnLan::HostState::Offline:
            if(Qt::DecorationRole == role)
                val = QIcon::fromTheme("network-offline");
            if(Qt::DisplayRole == role)
                val = tr("Offline");
            break;
        case CParameterWakeOnLan::HostState::GetMac:
            if(Qt::DecorationRole == role)
                val = QIcon::fromTheme("mac");
            if(Qt::DisplayRole == role)
                val = tr("Get mac ...");
            break;
        case CParameterWakeOnLan::HostState::WakeOnLan:
            if(Qt::DecorationRole == role)
                val = QIcon::fromTheme("lan");
            if(Qt::DisplayRole == role)
                val = tr("Wake on lan ...");
        default:
            break;
        }
    } else {
        if(Qt::DisplayRole == role)
        {
            if(1 == c)
                val = para->m_Net.GetHost();
            else if(2 == c)
                val = para->GetMac();
            else if(3 == c)
                val = para->GetBroadcastAddress();
            else if(4 == c)
                val = para->GetNetworkInterface();
            else if(5 == c)
                val = para->GetPort();
        }
    }
    return val;
}

bool CWakeOnLanModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug(log) << Q_FUNC_INFO << index << value << role;
    if(!index.isValid()) {
        return false;
    }
    int r = index.row();
    int c = index.column();
    if(r >= m_Data.size() || c >= m_Colume)
        return false;
    if(value.isNull() || value.toString().isEmpty())
        return false;
    auto p = m_Data.at(r);
    if(1 == c)
    {
        p->m_Net.SetHost(value.toString());
        foreach(auto iface, QNetworkInterface::allInterfaces()) {
            //qDebug(log) << iface;
            auto entry = iface.addressEntries();
            if(iface.flags() & QNetworkInterface::IsLoopBack)
                continue;
            if(!(iface.flags() & QNetworkInterface::CanBroadcast))
                continue;
            QString szBroadcast;
            foreach(auto e, entry) {
                if(!e.broadcast().isNull()) {
                    if(CParameterWakeOnLanUI::GetSubNet(
                            p->m_Net.GetHost(),
                            e.netmask().toString())
                        == CParameterWakeOnLanUI::GetSubNet(
                            e.ip().toString(),
                            e.netmask().toString()))
                    {
                        p->SetNetworkInterface(e.ip().toString());
                        p->SetBroadcastAddress(e.broadcast().toString());
                        break;
                    }
                }
            }
        }
    }
    else if(2 == c)
        p->SetMac(value.toString());
    else if(3 == c)
        p->SetBroadcastAddress(value.toString());
    else if(4 == c)
        p->SetNetworkInterface(value.toString());
    else if(5 == c)
        p->SetPort(value.toInt());
    return true;
}

int CWakeOnLanModel::AddItem(QSharedPointer<CParameterWakeOnLan> para)
{
    if(!para)
        return -1;
    QString szIp = para->m_Net.GetHost();
    foreach(auto p, m_Data)
    {
        if(p->m_Net.GetHost() == szIp){
            qDebug(log) << szIp << "is existed";
            return 0;
        }
    }
    bool check = connect(para.data(), SIGNAL(sigHostStateChanged()),
                         this, SLOT(slotHostStateChanged()));
    Q_ASSERT(check);
    beginResetModel();
    m_Data.push_back(para);
    endResetModel();
    return 0;
}

bool CWakeOnLanModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug(log) << Q_FUNC_INFO << row << count << parent;
    beginResetModel();
    m_Data.erase(m_Data.begin() + row);
    endResetModel();
    return true;
}

class CSortIp
{
public:
    CSortIp(Qt::SortOrder order, int column)
    {
        m_Order = order;
        m_column = column;
    }

    bool operator()(const QSharedPointer<CParameterWakeOnLan>& k1,
                    const QSharedPointer<CParameterWakeOnLan>& k2)
    {
        if(0 == m_column) {
            if(m_Order == Qt::AscendingOrder)
                return k1->GetHostState() < k2->GetHostState();
            else
                return k1->GetHostState() > k2->GetHostState();
        }
        if(1 == m_column) {
            if(m_Order == Qt::AscendingOrder)
                return k1->m_Net.GetHost() < k2->m_Net.GetHost();
            else
                return k1->m_Net.GetHost() > k2->m_Net.GetHost();
        }
        return false;
    }
private:
    Qt::SortOrder m_Order;
    int m_column;
};

void CWakeOnLanModel::sort(int column, Qt::SortOrder order)
{
    qDebug(log) << Q_FUNC_INFO << column << order;
    if(1 != column && 0 != column)
        return;
    if(m_Sort.find(column) != m_Sort.end())
        if(m_Sort[column] == order)
            return;
    m_Sort[column] = order;
    beginResetModel();
    CSortIp cmp(order, column);
    std::sort(m_Data.begin(), m_Data.end(), cmp);
    endResetModel();
}

int CWakeOnLanModel::Load(QSettings &set, CParameterClient* pClient)
{
    int nRet = 0;
    int count = 0;
    count = set.value("Count").toInt();
    for(int i = 0; i < count; i++)
    {
        QSharedPointer<CParameterWakeOnLan> p(new CParameterWakeOnLan());
        if(!p) {
            nRet = -1;
            break;
        }
        p->SetParameterClient(pClient);
        set.beginGroup("Host" + QString::number(i));
        nRet = p->Load(set);
        set.endGroup();
        if(nRet)
            break;
        AddItem(p);
    }
    return nRet;
}

int CWakeOnLanModel::Save(QSettings &set)
{
    int nRet = 0;
    int i = 0;
    set.setValue("Count", (int)m_Data.size());
    foreach(auto d, m_Data)
    {
        set.beginGroup("Host" + QString::number(i++));
        nRet = d->Save(set);
        set.endGroup();
        if(nRet)
            break;
    }
    return nRet;
}

void CWakeOnLanModel::slotHostStateChanged()
{
    CParameterWakeOnLan* p = qobject_cast<CParameterWakeOnLan*>(sender());
    if(!p)
        return;
    QString szIp = p->m_Net.GetHost();
    for(int i = 0; i < m_Data.size(); i++)
    {
        if(m_Data.at(i)->m_Net.GetHost() == szIp) {
            emit dataChanged(index(i, 0), index(i, m_Colume));
            break;
        }
    }
}

QSharedPointer<CParameterWakeOnLan> CWakeOnLanModel::GetData(const QModelIndex &index)
{
    if(index.row() < 0 || index.row() > m_Data.size())
        return nullptr;
    return m_Data.at(index.row());
}
