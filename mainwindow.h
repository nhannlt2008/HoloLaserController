#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateArduino(QByteArray command);
    void readData();

    void on_btnShot_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 1659;//9025;
    static const quint16 arduino_uno_product_id = 8963;//68;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray buffer;

};

#endif // MAINWINDOW_H
