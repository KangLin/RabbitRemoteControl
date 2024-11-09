// Author: Kang Lin <kl222@126.com>
#include <QLoggingCategory>

#ifdef HAVE_PCAPPLUSPLUS
    #include <Packet.h>
    #include <EthLayer.h>
    #include <ArpLayer.h>
    #include <PcapLiveDeviceList.h>
    #include <PcapFilter.h>
    #ifdef _MSC_VER
        #include <SystemUtils.h>
    #endif
#endif

#include "Arp.h"

static Q_LOGGING_CATEGORY(log, "WOL")
CArp::CArp(QObject *parent) : QObject(parent)
{
    qDebug(log) << __FUNCTION__;

    m_Timer.setInterval(1000);
    bool check = connect(&m_Timer, SIGNAL(timeout()),
                         this, SLOT(slotProcess()));
    Q_ASSERT(check);
}

CArp::~CArp()
{
    qDebug(log) << __FUNCTION__;
    m_Mutex.lock();
    foreach(auto a,  m_ArpRequest)
    {
        StopCapture(a->device, !a->bOpen);
    }
    m_Mutex.unlock();
}

int CArp::WakeOnLan(QSharedPointer<CParameterWakeOnLan> para)
{
    int nRet = 0;

    if(para->GetMac().isEmpty())
    {
        qCritical(log) << "The mac address is empty";
        return -1;
    }

    auto it = m_ArpRequest.find(para->m_Net.GetHost());
    if(it != m_ArpRequest.end())
    {
        if((*it)->bWakeOnLan) {
            qDebug(log) << "The wake on lan is existed"
                        << (*it)->para->m_Net.GetHost();
            return 0;
        }
    }

    m_Wol.SetBroadcastAddress(para->GetBroadcastAddress());
    if(para->GetPassword().isEmpty())
        m_Wol.SendMagicPacket(para->GetMac());
    else
        m_Wol.SendSecureMagicPacket(para->GetMac(), para->GetPassword());

    QSharedPointer<ArpRequest> aq(new ArpRequest());
    if(!aq) {
        qCritical(log) << "new ArpRequest fail";
        return -2;
    }
    aq->bWakeOnLan = true;
    aq->nRepeat = para->GetRepeat();
    nRet = GetMac(para, aq);
    return nRet;
}

#ifdef HAVE_PCAPPLUSPLUS
int CArp::StopCapture(pcpp::PcapLiveDevice *device, bool bClose)
{
    // stop the capturing thread
    device->stopCapture();
    if(bClose)
        device->close();
    return 0;
}

class ArpingReceivedData : QObject
{
public:
    ArpingReceivedData(CArp* p, QSharedPointer<CArp::ArpRequest> a)
    {
        pThis = p;
        aq = a;
    }
    ~ArpingReceivedData()
    {
        qDebug(log) << __FUNCTION__;
    }
    CArp* pThis;
    QSharedPointer<CArp::ArpRequest> aq;
};

static void cbArpPacketReceived(pcpp::RawPacket* rawPacket,
                                pcpp::PcapLiveDevice*, void* userCookie)
{
    using namespace pcpp;

    // parse the response packet
    Packet packet(rawPacket);

    // verify that it's an ARP packet
    // (although it must be because I set an ARP reply filter on the interface)
    if (!packet.isPacketOfType(ARP))
        return;

    // extract the ARP layer from the packet
    ArpLayer* arpReplyLayer = packet.getLayerOfType<ArpLayer>(true);  // lookup in reverse order
    if (arpReplyLayer == nullptr)
        return;

    // verify it's the right ARP response
    if (arpReplyLayer->getArpHeader()->hardwareType != hostToNet16(1) /* Ethernet */
        || arpReplyLayer->getArpHeader()->protocolType != hostToNet16(PCPP_ETHERTYPE_IP))
        return;

    // get the data from the main thread
    ArpingReceivedData* pData = reinterpret_cast<ArpingReceivedData*>(userCookie);
    if(!pData || !pData->aq || !pData->pThis || !pData->aq->para)
    {
        qDebug(log) << "The use data is nullptr";
        return;
    }
    QSharedPointer<CParameterWakeOnLan> para = pData->aq->para;

    // verify the ARP response is the response for out request
    // (and not some arbitrary ARP response)
    if (arpReplyLayer->getSenderIpAddr().toString()
        != para->m_Net.GetHost().toStdString())
        return;

    para->SetMac(arpReplyLayer->getSenderMacAddress().toString().c_str());
    para->SetHostState(CParameterWakeOnLan::HostState::Online);

    pData->pThis->m_Mutex.lock();
    // See: CArp::slotProcess()
    pData->aq->nTimeout = 0;
    pData->pThis->m_Mutex.unlock();
    delete pData;
}
#endif

int CArp::GetMac(QSharedPointer<CParameterWakeOnLan> para,
                 QSharedPointer<ArpRequest> aq)
{
    qDebug(log) << __FUNCTION__ << para;

#ifdef HAVE_PCAPPLUSPLUS
    using namespace pcpp;
    std::string szSourceIp = para->GetNetworkInterface().toStdString();
    std::string szTargetIp = para->m_Net.GetHost().toStdString();
    pcpp::MacAddress sourceMac;
    pcpp::IPv4Address sourceIP(szSourceIp);
    pcpp::IPv4Address targetIP(szTargetIp);
    pcpp::PcapLiveDevice* device = nullptr;
    try{
        if(!pcpp::IPv4Address::isValidIPv4Address(szTargetIp)) {
            qCritical(log) << "Target ip is invalid:" << szTargetIp;
            return -1;
        }

        device = pcpp::PcapLiveDeviceList::getInstance()
                     .getPcapLiveDeviceByIp(sourceIP);
        if (device == nullptr) {
            qCritical(log)
                << "Couldn't find interface by provided IP address or name"
                << szSourceIp;
            return -2;
        }

        if(!aq)
            aq = QSharedPointer<ArpRequest>(new ArpRequest());
        aq->bOpen = device->isOpened();
        aq->device = device;
        aq->para = para;
        aq->nTimeout = para->GetTimeOut();

        if(!device->isOpened())
            if(!device->open()) {
                qCritical(log) << "Open device fail" << szSourceIp;
                return -3;
            }

        if(!pcpp::IPv4Address::isValidIPv4Address(szSourceIp))
            sourceIP = device->getIPv4Address();
        sourceMac = device->getMacAddress();

        // create an ARP request from sourceMac and sourceIP and ask for target IP
        Packet arpRequest(100);
        MacAddress destMac(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        EthLayer ethLayer(sourceMac, destMac);
        ArpLayer arpLayer(ARP_REQUEST, sourceMac, destMac, sourceIP, targetIP);
        if (!arpRequest.addLayer(&ethLayer))
        {
            qCritical(log) << "Couldn't build Eth layer for ARP request";
            return -4;
        }
        if (!arpRequest.addLayer(&arpLayer))
        {
            qCritical(log) << "Couldn't build ARP layer for ARP request";
            return -5;
        }
        arpRequest.computeCalculateFields();

        // set a filter for the interface to intercept only ARP response packets
        ArpFilter arpFilter(ARP_REPLY);
        if (!device->setFilter(arpFilter))
        {
            qCritical(log) << "Couldn't set ARP filter for device";
            return -6;
        }

        aq->tmStart = QTime::currentTime();
        m_Mutex.lock();
        m_ArpRequest.insert(szTargetIp.c_str(), aq);
        m_Mutex.unlock();

        ArpingReceivedData* data = new ArpingReceivedData(this, aq);

        // start capturing. The capture is done on another thread,
        // hence "arpPacketReceived" is running on that thread
        device->startCapture(cbArpPacketReceived, data);

        // send the ARP request
        device->sendPacket(&arpRequest);

        if(!m_Timer.isActive())
            m_Timer.start();

    } catch(std::exception e) {
        qDebug(log) << "std::exception" << e.what();
    } catch(...) {
        qDebug(log) << "Exception";
    }
#endif
    return 0;
}

void CArp::ListInterfaces()
{
#ifdef HAVE_PCAPPLUSPLUS
    const std::vector<pcpp::PcapLiveDevice*>& devList =
        pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();

    qDebug(log) << "Network interfaces:";
    for (const auto& dev : devList)
    {
        qDebug(log) << "    -> Name: '"
                    << dev->getName().c_str() << "'   IP address: "
                    << dev->getIPv4Address().toString().c_str();
    }
#endif
    return;
}

void CArp::slotProcess()
{
#ifdef HAVE_PCAPPLUSPLUS
    m_Mutex.lock();
    for(auto it = m_ArpRequest.begin(); it != m_ArpRequest.end();)
    {
        auto a = *it;
        auto para = a->para;

        // Wake on lan
        if(a->bWakeOnLan && a->nRepeat
            && a->tmRepeat.msecsTo(QTime::currentTime()) > para->GetInterval())
        {
            m_Wol.SetBroadcastAddress(para->GetBroadcastAddress());
            if(para->GetPassword().isEmpty())
                m_Wol.SendMagicPacket(para->GetMac());
            else
                m_Wol.SendSecureMagicPacket(para->GetMac(), para->GetPassword());

            a->tmRepeat = QTime::currentTime();
            a->nRepeat--;
        }

        // Get mac address fail
        if(a->tmStart.msecsTo(QTime::currentTime()) > a->nTimeout) {
            StopCapture(a->device, !a->bOpen);
            QString szTargetIp = a->para->m_Net.GetHost();
            m_ArpRequest.remove(szTargetIp);
            it = m_ArpRequest.begin();
            // See: cbArpPacketReceived
            if(a->nTimeout)
                para->SetHostState(CParameterWakeOnLan::HostState::Offline);
        } else
            it++;
    }
    m_Mutex.unlock();
    if(m_ArpRequest.isEmpty())
        m_Timer.stop();
#endif
}
