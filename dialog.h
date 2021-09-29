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
    QSerialPort m_serial;

private slots:
    void on_m_btnOpenUART_clicked();
    void readData();

    // QObject interface
    void on_m_btnReadRegMDIO0_clicked();
};
#endif // DIALOG_H
