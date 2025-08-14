#include <QLoggingCategory>
#include <QSerialPortInfo>
#include "ParameterPlugin.h"
#include "DlgSettingsSerialPort.h"
#include "ui_DlgSettingsSerialPort.h"

static Q_LOGGING_CATEGORY(log, "SerialPort.Dlg")
CDlgSettingsSerialPort::CDlgSettingsSerialPort(CParameterSerialPort *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsSerialPort)
    , m_pPara(pPara)
{
    ui->setupUi(this);

    m_pFrmParaAppearnce =
        new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                              m_pFrmParaAppearnce->windowTitle());
    }

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        /*
        qDebug(log)
                 << "Port:" << info.portName() << " "
                 << "Location:" << info.systemLocation() << " "
                 << "Description:" << info.description() << " "
                 << "Manufacturer:" << info.manufacturer() << " "
                 << "Serial number:" << info.serialNumber() << " "
                 << "Vendor Identifier:"
                 << (info.hasVendorIdentifier()
                         ? QByteArray::number(info.vendorIdentifier(), 16)
                         : QByteArray()) << " "
                 << "Product Identifier:"
                 << (info.hasProductIdentifier()
                         ? QByteArray::number(info.productIdentifier(), 16)
                         : QByteArray()); //*/
        QString szPort;
        szPort = info.portName();
        //szPort = info.systemLocation();
        // if(!info.description().isEmpty())
        // {
        //     szPort += "(" + info.description() + ")";
        // }
        ui->cbSerialPort->addItem(szPort);
    }
    int nSerialPort = m_pPara->GetSerialPort();
    QSerialPortInfo info;
    if(-1 < nSerialPort && nSerialPort < ui->cbSerialPort->count()) {
        ui->cbSerialPort->setCurrentIndex(nSerialPort);
    }
    if(-1 != ui->cbSerialPort->currentIndex())
        info = QSerialPortInfo::availablePorts().at(ui->cbSerialPort->currentIndex());
    QSerialPort serialPort(info);

    if(!m_pPara->GetSerialPortName().isEmpty()
        && ui->cbSerialPort->currentText() != m_pPara->GetSerialPortName()) {
        ui->cbSerialPort->setCurrentText(m_pPara->GetSerialPortName());
        serialPort.setPortName(m_pPara->GetSerialPortName());
    }

    foreach(const qint32 &baudRate, QSerialPortInfo::standardBaudRates())
    {
        ui->cbBaudRate->addItem(QString::number(baudRate));
    }
    QString szBaudRate;
    if(-1 == nSerialPort)
        szBaudRate = QString::number(serialPort.baudRate());
    else
        szBaudRate = QString::number(m_pPara->GetBaudRate());
    ui->cbBaudRate->setCurrentText(szBaudRate);

    ui->cbDataBits->addItem("8", QSerialPort::DataBits::Data8);
    ui->cbDataBits->addItem("7", QSerialPort::DataBits::Data7);
    ui->cbDataBits->addItem("6", QSerialPort::DataBits::Data6);
    ui->cbDataBits->addItem("5", QSerialPort::DataBits::Data5);
    QSerialPort::DataBits dataBits;
    if(-1 == nSerialPort)
        dataBits = serialPort.dataBits();
    else
        dataBits = m_pPara->GetDataBits();
    int nIndex = ui->cbDataBits->findData(dataBits);
    ui->cbDataBits->setCurrentIndex(nIndex);
    
    ui->cbParity->addItem(tr("None"), QSerialPort::Parity::NoParity);
    ui->cbParity->addItem(tr("Even"), QSerialPort::Parity::EvenParity);
    ui->cbParity->addItem(tr("Odd"), QSerialPort::Parity::OddParity);
    ui->cbParity->addItem(tr("Space"), QSerialPort::Parity::SpaceParity);
    ui->cbParity->addItem(tr("Mark"), QSerialPort::Parity::MarkParity);
    QSerialPort::Parity parity;
    if(-1 == nSerialPort)
        parity = serialPort.parity();
    else
        parity = m_pPara->GetParity();
    nIndex = ui->cbParity->findData(parity);
    ui->cbParity->setCurrentIndex(nIndex);

    ui->cbStopBits->addItem("1", QSerialPort::StopBits::OneStop);
    ui->cbStopBits->addItem("1.5", QSerialPort::StopBits::OneAndHalfStop);
    ui->cbStopBits->addItem("2", QSerialPort::StopBits::TwoStop);
    QSerialPort::StopBits stopBits;
    if(-1 == nSerialPort)
        stopBits = serialPort.stopBits();
    else
        stopBits = m_pPara->GetStopBits();
    nIndex = ui->cbStopBits->findData(stopBits);
    ui->cbStopBits->setCurrentIndex(nIndex);

    ui->cbFlowControl->addItem(tr("None"), QSerialPort::FlowControl::NoFlowControl);
    ui->cbFlowControl->addItem(tr("HardWare"), QSerialPort::FlowControl::HardwareControl);
    ui->cbFlowControl->addItem(tr("SoftWare"), QSerialPort::FlowControl::SoftwareControl);
    QSerialPort::FlowControl flowControl;
    if(-1 == nSerialPort)
        flowControl = serialPort.flowControl();
    else
        flowControl = m_pPara->GetFlowControl();
    nIndex = ui->cbFlowControl->findData(flowControl);
    ui->cbFlowControl->setCurrentIndex(nIndex);
}

CDlgSettingsSerialPort::~CDlgSettingsSerialPort()
{
    delete ui;
}

void CDlgSettingsSerialPort::accept()
{
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();
    
    m_pPara->SetSerialPort(ui->cbSerialPort->currentIndex());
    m_pPara->SetSerialPortName(ui->cbSerialPort->currentText());
    m_pPara->SetBaudRate(ui->cbBaudRate->currentText().toInt());
    m_pPara->SetParity((QSerialPort::Parity)ui->cbParity->currentData().toInt());
    m_pPara->SetDataBits((QSerialPort::DataBits)ui->cbDataBits->currentData().toInt());
    m_pPara->SetStopBits((QSerialPort::StopBits)ui->cbStopBits->currentData().toInt());
    m_pPara->SetFlowControl((QSerialPort::FlowControl)ui->cbFlowControl->currentData().toInt());
    QDialog::accept();
}
