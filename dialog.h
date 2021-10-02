#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "MDIO_UART.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;

protected:
    virtual void  FillCheckBoxesChannel0();
    virtual uint32_t ReadMDIORegister (int nPhy,int nReg);
    QSerialPort m_serial;

    uint8_t m_receivedData [0x200];
    uint32_t m_receivedPtr;
    enum
    {
        rcvBhReadMdioRegisterPhy0 = 0,
        rcvBhReadMdioRegisterPhy1,
        rcvBhNoNeedReaction
    } m_rcvBehaviour;
private slots:
    void on_m_btnOpenUART_clicked();
    void readData();

    // QObject interface
    void on_m_btnReadRegMDIO0_clicked();
    void on_m_btnReadRegMDIO1_clicked();
    void on_m_btnWriteRegMDIO0_clicked();
    void on_m_btnWriteRegMDIO1_clicked();
    void on_m_reset_phy_0_clicked();
};
#endif // DIALOG_H
