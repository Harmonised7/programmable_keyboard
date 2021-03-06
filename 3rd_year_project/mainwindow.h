#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "util.h"
#include "buttonData.h"
#include "prefs.h"

#include <QMainWindow>

// Qt includes
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //Methods
    void initItemTree();
    void updateItemsTree();
    void updateItemsTreeIndices();
    void print(QString string);
    ButtonData *getSelectedButtonData();
    QJsonObject getJsonData();
    QJsonObject getInfoJson();
    QString getDataType(QTreeWidgetItem *item);
    int getEntryIndex(QTreeWidgetItem *item);
    bool isParentOf(QTreeWidgetItem *potentialChild, QTreeWidgetItem *potentialParent);
    QString chooseJsonSavePath(QString startPath = "./");
    QString chooseJsonReadPath(QString startPath = "./");

public slots:
    void buttonPress();
    void selectButton( int index );

private slots:
    void on_addActionButton_clicked();

    void on_buttonInfoTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_removeActionButton_clicked();

    void on_buttonInfoTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_dataTypeBox_currentIndexChanged(int index);

    void on_writeButton_clicked();

    void on_actionExport_triggered();

    void on_actionImport_triggered();

    void on_actionClear_triggered();

    void readSerial();

    void onReadFinished();

    void writeQueuedSerial();

    void on_actionShow_Console_triggered(bool checked);

private:
    //Static Fields
    Ui::MainWindow *ui;
    const QString BUTTONS = "buttons",
                  INFO = "info";
    QString jsonFilter = "JSON (*.json)";
    const quint16 _ARDUINO_UNO_VENDOR_ID = 9025;
    const quint16 _ARDUINO_UNO_PRODUCT_ID = 67;

    const quint16 _SPARKFUN_PRO_MICRO_VENDOR_ID = 6991;
    const quint16 _SPARKFUN_PRO_MICRO_PRODUCT_ID = 37382;

    //Fields
    QList<QByteArray> _queuedSerialToSend;
    QList<QPushButton*> _buttons;
    const float _bSize = 100;
    int _selectedButtonIndex = 0;
    QMap<QString, QTreeWidgetItem*> _treeItemsMap;
//    QString _arduinoPortName;
//    bool _isArduinoAvailable = false;
    QSerialPort *_mcu_serial;
    QByteArray _serialData;
    QString _serialBuffer, _parsedData;
    int _debugInt = 0, _debugInt2 = 0;
    int _SERIAL_READ_TIMEOUT = 500;
    int _SERIAL_WRITE_TIMEOUT = 1000;
    QTimer _serialTimeoutTimer;
    QTimer _sendQueuedSerialTimer;

    // Methods
    void attemptArduinoConnection();
    void writeButtonToSerial(int buttonIndex);
};
#endif // MAINWINDOW_H
