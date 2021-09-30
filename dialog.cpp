#include "dialog.h"
#include "ui_dialog.h"
#include <QSettings>
#include <QSerialPortInfo>

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
    s.reserved = 0;
    s.data = 0;
    m_serial.write((char*)&s,sizeof(s));
}
