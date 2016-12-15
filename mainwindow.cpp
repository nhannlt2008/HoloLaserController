#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtDebug>
#include <QtWidgets>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;


    qDebug() << " Number of available ports: " << QSerialPortInfo::availablePorts().length();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasVendorIdentifier()) {
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier()) {
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }

   foreach(const QSerialPortInfo &serialPortInfo,QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {
            if (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id) {
                if (serialPortInfo.productIdentifier() == arduino_uno_product_id) {
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }
    }

    if (arduino_is_available) {
        arduino->setPortName(arduino_port_name);
        arduino->setBaudRate(QSerialPort::Baud38400);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        //arduino->set

        arduino->open(QSerialPort::ReadWrite);

        connect(arduino,SIGNAL(readyRead()),this,SLOT(readData()));
        //arduino->open(QIODevice::ReadWrite);

    } else {
        QMessageBox::warning(this," Port error", "Couldn't find the Arduino");
    }
}

void MainWindow::updateArduino(QByteArray command)
{
    ui->edtResult->setText(command);
    if (arduino->isWritable()) {
        arduino->write(command);
    } else {
        qDebug() << "Couldn't write to serial";
    }   
}

void MainWindow::readData()
{
    QByteArray readBytes = arduino->readAll();
    if ( readBytes[0] == 0xAA) {
        buffer.clear();
    }
    buffer.append(readBytes);
    if ( buffer.length() >= 8) {
        unsigned int distance = (unsigned char)(buffer[3]) << 16 |
                                                                  (unsigned char)(buffer[4]) << 8 |
                                                                  (unsigned char)(buffer[5]);
        if (buffer[2] == 0x06) {
            ui->edtResult->setText("Distance");
            ui->edtResult->append(QString::number(distance).toStdString().c_str());
            ui->edtResult->append(" mm");
        }
        else {
            switch ((unsigned char)buffer[2]) {
                case 0x15:  ui->edtResult->setText("Complete abnormal function!");break;
                case 0xF0:  ui->edtResult->setText("Test signal is weak!");break;
                case 0xF1:  ui->edtResult->setText("Out of range!"); break;
            }
        }
       ui->edtResult->append(buffer.toHex());
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnShot_clicked()
{
    static const char cmd[] =
            {0xAA,0x03,0x00,0x00,0x00,0x00,0xAD};
    QByteArray bytes = QByteArray::fromRawData(
        cmd,7);
    MainWindow::updateArduino(bytes);

}
