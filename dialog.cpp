#include "dialog.h"
#include "ui_dialog.h"
#include <QSettings>
#include <QSerialPortInfo>
#include <QThread>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QSettings settings( "YolkaPlay.conf", QSettings::IniFormat );
    settings.beginGroup( "DefaultPort" );
    QString defaultPortName = settings.value("Name",QVariant("Nothing")).toString();
    settings.endGroup();

    int index = 0;
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->m_listUARTs->addItem(serialPortInfo.portName());
        if (defaultPortName == serialPortInfo.portName())
        {
            ui->m_listUARTs->setCurrentIndex(index);
        }
        index += 1;
    }

    connect(&m_serial, &QSerialPort::readyRead, this, &Dialog::readData);
// Just for test

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::FillCheckBoxesChannel0()
{
    uint32_t reg04 = ReadMDIORegister(0,4);
    ui->m_cbTx0_10H->setChecked((reg04 & (1<<5)) != 0);
    ui->m_cbTx0_10F->setChecked((reg04 & (1<<6)) != 0);
    ui->m_cbTx0_100H->setChecked((reg04 & (1<<7)) != 0);
    ui->m_cbTx0_100F->setChecked((reg04 & (1<<8)) != 0);
    uint32_t reg00 = ReadMDIORegister(0,0);
    ui->m_cbAutoNeg_0->setChecked((reg00 & (1<<12)) != 0);
    ui->m_cb100M_0->setChecked((reg00 & (1<<13)) != 0);
    ui->m_cbFullDuplex_0->setChecked((reg00 & (1<<8)) != 0);
/*    uint32_t reg19 = ReadMDIORegister(0,0x19);
    ui->m_cbCrossCable_0->setChecked((reg19 & (1<<14)) != 0);
    ui->m_cbAutoCross_0->setChecked((reg19 & (1<<15)) != 0);*/

}

uint32_t Dialog::ReadMDIORegister(int nPhy, int nReg)
{
    MdioUartStruct s;
    s.cmd = MDIO_CMD_READ;
    s.phyAddr = nPhy;
    s.regAddr = nReg;
    s.reserved = 0;
    s.data = 0;

    // Flush all UART Data
    m_receivedPtr = 0;
    m_serial.clear(QSerialPort::Input);

    // Switch receiver mode
    m_rcvBehaviour = rcvBhReadMdioRegisterPhy0;
    // Send command
    m_serial.write((char*)&s,sizeof(s));

    //TODO: Add timeout catch!!!
    while (m_receivedPtr < 2)
    {
        QThread::msleep(2);
        QCoreApplication::processEvents();
    }

    return m_receivedData[0] + m_receivedData[1] * 0x100;
}


void Dialog::on_m_btnOpenUART_clicked()
{
    if (m_serial.isOpen())
    {
        m_serial.close();
        ui->m_btnOpenUART->setText("Open");
        ui->m_btnOpenUART->setIcon(QIcon(":/new/icons/Icons/LedGray.png"));
    } else
    {
        m_serial.setPortName(ui->m_listUARTs->currentText());
        m_serial.setBaudRate(115200);
        m_serial.setDataBits(QSerialPort::Data8);
        m_serial.setParity(QSerialPort::NoParity);
        m_serial.setStopBits(QSerialPort::OneStop);
        m_serial.setFlowControl(QSerialPort::NoFlowControl);
        if (m_serial.open (QSerialPort::ReadWrite))
        {
            ui->m_btnOpenUART->setText("Close");
            ui->m_btnOpenUART->setIcon(QIcon(":/new/icons/Icons/LedGreen.png"));

            QSettings settings( "YolkaPlay.conf", QSettings::IniFormat );
            settings.beginGroup( "DefaultPort" );
            settings.setValue( "Name", ui->m_listUARTs->currentText() );
            settings.endGroup();

        } else
        {
            ui->m_btnOpenUART->setIcon(QIcon(":/new/icons/Icons/LedRed.png"));
        }
    }

}

void Dialog::readData()
{
    QByteArray data = m_serial.readAll();
    for (int i=0;i<data.size();i++)
    {
        if (m_receivedPtr < sizeof(m_receivedData))
        {
            m_receivedData [m_receivedPtr++] = data.at(i);
        }
    }
    switch (m_rcvBehaviour)
    {
    case rcvBhReadMdioRegisterPhy0:
    {
        if (m_receivedPtr >= 2)
        {
            uint32_t data = m_receivedData [0] + m_receivedData [1] * 0x100;
            ui->m_regReadDataForMDIO0->setText(QString("%1").arg(data, 4, 16, QChar('0')));
        }
    }
    break;
    case rcvBhReadMdioRegisterPhy1:
    {
        if (m_receivedPtr >= 2)
        {
            uint32_t data = m_receivedData [0] + m_receivedData [1] * 0x100;
            ui->m_regReadDataForMDIO1->setText(QString("%1").arg(data, 4, 16, QChar('0')));
        }

    }
    break;
    default:
        break;
    }

/*    m_log.write(data);
    uint32_t time;
    if (m_gpsParser.ParseData(data,time))
    {
        ui->m_curTime->setText(QString::number(time));

        int coarseTime = time / 1000;
        int seconds = coarseTime % 60;
        int minutes = (coarseTime / 60) % 60;
        int hours = (coarseTime / (60 * 60)) % 24;
        ui->m_curTime_2->setText (QString ("%1:%2:%3").arg(hours,2,10,QChar('0')).arg(minutes,2,10,QChar('0')).arg(seconds,2,10,QChar('0')));
    }*/
}

void Dialog::on_m_btnReadRegMDIO0_clicked()
{
    bool bOK;
    uint16_t reg = ui->m_regNrForMDIO0->text().toUInt(&bOK,16);

    MdioUartStruct s;
    s.cmd = MDIO_CMD_READ;
    s.phyAddr = 0;
    s.regAddr = reg;
    s.reserved = ui->m_comboSpeed_0->currentIndex();
    s.data = 0;

    // Flush all UART Data
    m_receivedPtr = 0;
    // Switch receiver mode
    m_rcvBehaviour = rcvBhReadMdioRegisterPhy0;
    // Send command
    m_serial.write((char*)&s,sizeof(s));
    // Answer will be handled inside callback function
}

void Dialog::on_m_btnReadRegMDIO1_clicked()
{
    bool bOK;
    uint16_t reg = ui->m_regNrForMDIO1->text().toUInt(&bOK,16);

    MdioUartStruct s;
    s.cmd = MDIO_CMD_READ;
    s.phyAddr = 1;
    s.regAddr = reg;
    s.reserved = ui->m_comboSpeed_0->currentIndex();
    s.data = 0;

    // Flush all UART Data
    m_receivedPtr = 0;
    // Switch receiver mode
    m_rcvBehaviour = rcvBhReadMdioRegisterPhy1;
    // Send command
    m_serial.write((char*)&s,sizeof(s));
    // Answer will be handled inside callback function

}

void Dialog::on_m_btnWriteRegMDIO0_clicked()
{
    bool bOK;
    uint16_t reg = ui->m_regNrForMDIO0->text().toUInt(&bOK,16);

    MdioUartStruct s;
    s.cmd = MDIO_CMD_WRITE;
    s.phyAddr = 0;
    s.regAddr = reg;
    s.reserved = 0;
    s.data = ui->m_regReadDataForMDIO0->text().toUInt(&bOK,16);

    // Flush all UART Data
    m_receivedPtr = 0;
    // Switch receiver mode
    m_rcvBehaviour = rcvBhNoNeedReaction;
    // Send command
    m_serial.write((char*)&s,sizeof(s));

}

void Dialog::on_m_btnWriteRegMDIO1_clicked()
{
    bool bOK;
    uint16_t reg = ui->m_regNrForMDIO1->text().toUInt(&bOK,16);

    MdioUartStruct s;
    s.cmd = MDIO_CMD_WRITE;
    s.phyAddr = 1;
    s.regAddr = reg;
    s.reserved = 0;
    s.data = ui->m_regReadDataForMDIO1->text().toUInt(&bOK,16);

    // Flush all UART Data
    m_receivedPtr = 0;
    // Switch receiver mode
    m_rcvBehaviour = rcvBhNoNeedReaction;
    // Send command
    m_serial.write((char*)&s,sizeof(s));

}

void Dialog::on_m_reset_phy_0_clicked()
{
    FillCheckBoxesChannel0();
}
