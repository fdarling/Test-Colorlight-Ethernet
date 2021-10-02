#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "MDIO_UART.h"
#include "pcap-int.h"
#include "Packet32.h"

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
    pcap_t* m_hCardSource;
    uint8_t m_macSource[6];

    struct addrAndPort
    {
        uint8_t* mac;
        uint32_t ip;
        uint32_t port;
    };

    struct udpData
    {
        uint8_t* m_pData;
        uint8_t* m_pUserData;
        int      m_userSize;
        int      m_totalDataSize;
        udpData ()
        {
            m_pData = 0;
            m_userSize = 0;
            m_totalDataSize = 0;
        }
        ~udpData ()
        {
            if (m_pData != 0)
            {
                delete[] m_pData;
            }
        }
        void SetUserSize (int userSizeInBytes)
        {
            if (m_userSize == userSizeInBytes)
            {
                return;
            }
            if (m_pData != 0)
            {
                delete[] m_pData;
            }
            m_totalDataSize =userSizeInBytes + 42;
            m_pData = new uint8_t [m_totalDataSize];

            m_pUserData = m_pData + 42;
            m_userSize = userSizeInBytes;
        }
        uint8_t& operator[](int index)
        {
            return m_pData[index];
        }
        inline int GetUserSize(){return m_userSize;}
    };

    unsigned short BytesTo16(unsigned char X,unsigned char Y)
    {
             unsigned short Tmp = X;
             Tmp = Tmp << 8;
             Tmp = Tmp | Y;
             return Tmp;
    }
    unsigned int BytesTo32(unsigned char W,unsigned char X,unsigned char Y,unsigned char Z)
    {
             unsigned int Tmp = W;
             Tmp = Tmp << 8;
             Tmp = Tmp | X;
             Tmp = Tmp << 8;
             Tmp = Tmp | Y;
             Tmp = Tmp << 8;
             Tmp = Tmp | Z;
             return Tmp;
    }

    void CreatePacket
                         (  addrAndPort& source,
                            addrAndPort& destination,
                            udpData& packet
                         );
    unsigned short CalculateUDPChecksum(udpData& packet);
    unsigned short CalculateIPChecksum(udpData& packet/*,UINT TotalLen,UINT ID,UINT SourceIP,UINT DestIP*/);



    virtual void  FillCheckBoxesChannel0();
    virtual uint32_t ReadMDIORegister (int nPhy,int nReg);
    virtual void WriteMDIORegister(int nPhy, int nReg, uint32_t data);

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
    void on_m_btnUpdateUi_0_clicked();
    void on_m_cbTx0_100F_clicked();
    void on_m_cbTx0_100H_clicked();
    void on_m_cbTx0_10F_clicked();
    void on_m_cbTx0_10H_clicked();
    void on_m_cbAutoNeg_0_clicked();
    void on_m_cbFullDuplex_0_clicked();
    void on_m_comboSpeed_0_currentIndexChanged(int index);
    void on_m_btnOpenEthCard_clicked();
    void on_m_btnCloseEthCard_clicked();
    void on_m_btnSendPkt_clicked();
};
#endif // DIALOG_H
