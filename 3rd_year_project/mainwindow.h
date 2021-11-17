#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "util.h"
#include "buttonData.h"

#include <QMainWindow>

// Qt includes
#include <QPushButton>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initItemTree();

    void updateItemsTree();
    void updateItemsTreeIndices();
    ButtonData *getSelectedButtonData();
    QJsonObject getJsonData();
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

private:
    //Static Fields
    Ui::MainWindow *ui;
    inline static const QString BUTTONS = "buttons",
                                INFO = "info";
    inline static QString jsonFilter = "JSON (*.json)";
    static const quint16 _ARDUINO_UNO_VENDOR_ID = 9025;
    static const quint16 _ARDUINO_UNO_PRODUCT_ID = 67;

    //Fields
    QList<QPushButton*> _buttons;
    const float _bSize = 100;
    int _selectedButtonIndex = 0;
    QMap<QString, QTreeWidgetItem*> _treeItemsMap;
    QSerialPort *_arduino;
    QString _arduinoPortName;
    bool _isArduinoAvailable = false;

    // Methods
    // ...
};
#endif // MAINWINDOW_H
