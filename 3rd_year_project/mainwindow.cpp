#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "util.h"
#include "buttonData.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialize the tree
    initItemTree();

    //Initialize action types
    ButtonData::init();

    //Populate on press types
    for(QString key : ButtonData::DATA_KEYS)
    {
        ui->dataTypeBox->addItem(key);
    }

    //Clear a QoL text
    ui->nameLabel->clear();

    //Tell the vector the amount of obects that it will hold
    _buttons.reserve(Prefs::bCount);

    //Populate the buttons
    for(int row = 0; row < Prefs::bRow; ++row)
    {
        for(int col = 0; col < Prefs::bCol; ++col)
        {
            int index = row*Prefs::bCol + col;
            QPushButton *button = new QPushButton("Button " + QString::number(index + 1));
            button->setMinimumSize(_bSize, _bSize);
            button->setMaximumSize(_bSize, _bSize);

            connect(button, SIGNAL(clicked()), this, SLOT(buttonPress()));

            _buttons.emplace(index, button);
            ui->buttonLayout->addWidget(_buttons[ index ], row, col);
        }
    }
    selectButton(_selectedButtonIndex);

    //Load backup
    QJsonObject json = Util::parseJson(QFile("./backup.json")).object();
    QJsonValue jsonButtonsData = json.value(BUTTONS);
    if(jsonButtonsData.isObject())
    {
        ButtonData::setButtonsData(ButtonData::buttonsDataFromJson(jsonButtonsData.toObject()));
        updateItemsTree();
    }

    //Attempt MCU Connection
    _mcu_serial = new QSerialPort;
    attemptArduinoConnection();
    _serialTimeoutTimer.setSingleShot(true);
    _sendQueuedSerialTimer.setSingleShot(true);

    ui->outputTextBox->setVisible(false);

//    this->window()->adjustSize();
//    this->window()->setFixedSize(this->window()->size());
}

MainWindow::~MainWindow()
{
    //Save backup
    QJsonObject json;
    json.insert(BUTTONS, ButtonData::allButtonsToJson());
    Util::writeToFile(json, "./backup.json");

    for (const auto &bt : _buttons)
        delete bt;

    if(_mcu_serial->isOpen())
        _mcu_serial->close();

    delete ui;
}

void MainWindow::initItemTree()
{
    ui->buttonInfoTreeWidget->setAlternatingRowColors(true);
    ui->buttonInfoTreeWidget->setStyleSheet("alternate-background-color: #dddddd");

    ui->buttonInfoTreeWidget->header()->setVisible(true);
    ui->buttonInfoTreeWidget->header()->resizeSection(Prefs::numberRow, 10);

    for(QString key : ButtonData::DATA_KEYS)
    {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem();
        treeItem->setText(Prefs::keyRow, key);
        treeItem->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        ui->buttonInfoTreeWidget->addTopLevelItem(treeItem);
        treeItem->setExpanded(true);
        _treeItemsMap.insert(key, treeItem);
    }
}

void MainWindow::updateItemsTree()
{
    for(QString dataKey : ButtonData::DATA_KEYS)
    {
        QTreeWidgetItem *treeItem = _treeItemsMap.value(dataKey);
        Util::clearQTreeWidgetItemChildren(treeItem);
    }
    if(_selectedButtonIndex >= 0)
    {
        ButtonData *data = getSelectedButtonData();
        #ifdef DEBUG
        qDebug() << "Loading button" << _selectedButtonIndex+1;
        #endif
        for(QString dataKey : ButtonData::DATA_KEYS)
        {
            int i = 0;
            QTreeWidgetItem *treeItem = _treeItemsMap.value(dataKey);

            for(Entry *item : *data->getData(dataKey))
            {
                #ifdef DEBUG
                qDebug() << "\tLoading" << dataKey << item->type() << item->value() << '|' << item->properties()->size() << "properties";
                #endif
                treeItem->addChild(item->toTreeWidgetItem());
            }
        }
    }
    updateItemsTreeIndices();
}

void MainWindow::updateItemsTreeIndices()
{
    if(_selectedButtonIndex >= 0)
    {
        for(QString dataKey : ButtonData::DATA_KEYS)
        {
            QTreeWidgetItem *tree = _treeItemsMap.value(dataKey);
            for(int i = 0; i < tree->childCount(); i++)
                tree->child(i)->setText(Prefs::numberRow, QString::number(i+1));
        }
    }
}

void MainWindow::print(QString string)
{
    ui->outputTextBox->moveCursor(QTextCursor::End);
    ui->outputTextBox->insertPlainText(string);
    ui->outputTextBox->moveCursor(QTextCursor::End);
    qDebug() << string;
}

QString MainWindow::getDataType(QTreeWidgetItem *item)
{
    if(item->parent())
    {
        for(QString key : _treeItemsMap.keys())
        {
            if(isParentOf(item, _treeItemsMap.value(key)))
                return key;
        }
    }
    else
        qDebug() << "PARENTLESS TREE ITEM";
    return "";
}

int MainWindow::getEntryIndex(QTreeWidgetItem *item)
{
    QString dataType = getDataType(item);
    //If valid data type
    if(dataType.length() > 0)
    {
        QTreeWidgetItem *treeItem = _treeItemsMap.value(getDataType(item));
        //Entries can only be in second column
        if(item->columnCount() > 1)
            return treeItem->indexOfChild(item);
        else
            return -1;
    }
    else
        return -1;
}

bool MainWindow::isParentOf(QTreeWidgetItem *potentialChild, QTreeWidgetItem *potentialParent)
{
    QTreeWidgetItem *parent = potentialChild->parent();
    while(parent)
    {
        if(parent == potentialParent)
            return true;
        else
            parent = parent->parent();
    }
    return false;
}

QString MainWindow::chooseJsonSavePath(QString startPath)
{
    return QFileDialog::getSaveFileName(nullptr, "nowo", startPath, jsonFilter, &jsonFilter, QFileDialog::DontUseNativeDialog);
}

QString MainWindow::chooseJsonReadPath(QString startPath)
{
    return QFileDialog::getOpenFileName(nullptr, "nowo", startPath, jsonFilter, &jsonFilter, QFileDialog::DontUseNativeDialog);
}

ButtonData *MainWindow::getSelectedButtonData()
{
    return ButtonData::getButtonData(_selectedButtonIndex);
}

QJsonObject MainWindow::getJsonData()
{
    QJsonObject data;
    data.insert(INFO, getInfoJson());
    data.insert(BUTTONS, ButtonData::allButtonsToJson());
    return data;
}

QJsonObject MainWindow::getInfoJson()
{
    QJsonObject info;
    info.insert("row", Prefs::bRow);
    info.insert("col", Prefs::bCol);
    info.insert(INFO, info);
    return info;
}

void MainWindow::buttonPress()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    const int index = _buttons.indexOf(button);

    selectButton(index);
}

void MainWindow::selectButton(int index)
{
    QPushButton *button = _buttons.value(_selectedButtonIndex);
    button->setStyleSheet("");
    ui->nameLabel->clear();

    _selectedButtonIndex = index;
    button = _buttons.value(_selectedButtonIndex);
    button->setStyleSheet("background-color: green");
    ui->nameLabel->setText(button->text());

    updateItemsTree();
}

void MainWindow::on_addActionButton_clicked()
{
    QString dataType = ui->dataTypeBox->currentText();
    QString entryType = ui->entryTypeBox->currentText();
    if(!ButtonData::hasTemplate(dataType, entryType))
        return; //Do nothing if template does not exist
    ButtonData *data = getSelectedButtonData();
    EntryList *entries = data->getData(dataType);
    Entry *entry = ButtonData::generateTemplateEntry(dataType, entryType);
    #ifdef DEBUG
    qDebug() << "Adding Action" << entryType << "to button" << _selectedButtonIndex << dataType;
    #endif
    data->addEntry(dataType, entry);
    #ifdef DEBUG
    qDebug() << entries->size() << "entries";
    #endif
    updateItemsTree();
}

void MainWindow::on_buttonInfoTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if(item->parent() && column == Prefs::valueRow)
        ui->buttonInfoTreeWidget->editItem(item, column);
}

void MainWindow::on_removeActionButton_clicked()
{
    QList<QTreeWidgetItem*> items = ui->buttonInfoTreeWidget->selectedItems();
    for(QTreeWidgetItem *item : items)
    {
        int index = getEntryIndex(item);
        if(index < 0 && item->parent())
            index = getEntryIndex(item->parent());
        if(index > -1)
        {
            QString dataType = getDataType(item);
            #ifdef DEBUG
            qDebug() << "Deleting entry" << index << "from button" << _selectedButtonIndex+1 << dataType;
            #endif
            //Remove from data
            getSelectedButtonData()->getEntries(dataType)->remove(index);
            //Remove from tree
            QTreeWidgetItem *treeItem = _treeItemsMap.value(dataType);
            if(item->parent() == treeItem)
                treeItem->removeChild(item);
            else    //If selected item is a property of an entry - remove the parent
                treeItem->removeChild(item->parent());
        }
    }
    updateItemsTreeIndices();
}

void MainWindow::on_buttonInfoTreeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    QString key = item->text(Prefs::keyRow);
    QString value = item->text(Prefs::valueRow);
    QString dataType = getDataType(item);
    EntryList *entries = getSelectedButtonData()->getData(dataType);
    int index = getEntryIndex(item);
    if(index > -1)
    {
        Entry *entry = entries->value(index);
        if(entry->value() != value)
        {
            #ifdef DEBUG
            qDebug() << "changed" << key << "from" << entry->value() << "to" << value;
            #endif
            entry->setValue(value);
        }
    }
    else
    {
        QTreeWidgetItem *parent = item->parent();
        index = getEntryIndex(parent);
        Entry *entry = entries->value(index);
        #ifdef DEBUG
        qDebug() << "changed" << dataType << parent->text(Prefs::keyRow) << "property" << key << "from" << entry->property(key) << "to" << value;
        #endif
        entry->setProperty(key, value);
    }
}

void MainWindow::on_dataTypeBox_currentIndexChanged(int index)
{
    #ifdef DEBUG
    qDebug() << index << ui->dataTypeBox->itemText(index);
    #endif
    ui->entryTypeBox->clear();
    QList<QString> entryTypes = ButtonData::getTemplateKeys(ui->dataTypeBox->itemText(index));
    ui->entryTypeBox->addItems(entryTypes);
}


void MainWindow::on_writeButton_clicked()
{
    if(!_mcu_serial->isOpen())
        attemptArduinoConnection();
    if(!_mcu_serial->isOpen())
    {
        print("Error: Target device not found!");
        QMessageBox::warning(this, "No Target Device", "Target device not found!");
    }
    else if(!_mcu_serial->isWritable())
    {
        print("Error: Target device not in Write mode!");
        QMessageBox::warning(this, "Not Writable", "Target device not in Write mode!");
    }
    else
    {
        //Clear previously queued serial if any was still not send
        _queuedSerialToSend.clear();
        //For each button, queue write
        for(int buttonIndex = 0; buttonIndex < Prefs::bCount; ++buttonIndex)
        {
            ButtonData *buttonData = ButtonData::getButtonData(buttonIndex);
            if(!ui->sendEmptyCheckBox->checkState() && buttonData->getEntries(ButtonData::PRESS)->length() == 0)
                continue;
            QJsonObject json = buttonData->toJson();
            //Insert an index value so the MCU knows which button the data belongs to
            json.insert("i", QString::number(buttonIndex));

//            QString outputPath = "./output_";
//            outputPath.append(QString::number(buttonIndex));
//            outputPath.append(".json");
//            Util::writeToFile(json, outputPath);

            _queuedSerialToSend.push_back(Util::toByteArray(json));
            _serialTimeoutTimer.start(_SERIAL_WRITE_TIMEOUT);

//            QByteArray command = Util::toByteArray(json);
//            print("Writing button " + QString::number(buttonIndex) + "...\n");
        }
        writeQueuedSerial();
    }
}

void MainWindow::on_actionClear_triggered()
{
    bool isEmpty = true;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Data", "This will clear all the buttons. Are you sure you want to proceed?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        ButtonData::wipeButtonsData();
        updateItemsTree();
    }
}

void MainWindow::attemptArduinoConnection()
{
    qDebug() << QSerialPortInfo::availablePorts().length();

//    _isArduinoAvailable = false;
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
//        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
//        {
//            if(serialPortInfo.vendorIdentifier() == _SPARKFUN_PRO_MICRO_VENDOR_ID)
//            {
//                if(serialPortInfo.productIdentifier() == _SPARKFUN_PRO_MICRO_PRODUCT_ID)
//                {
//                    _arduinoPortName = serialPortInfo.portName();
//                    _isArduinoAvailable = true;
                    _mcu_serial->setPortName(serialPortInfo.portName());
                    _mcu_serial->setBaudRate(QSerialPort::Baud115200);
                    _mcu_serial->open(QSerialPort::ReadWrite);
                    _mcu_serial->setDataBits(QSerialPort::Data8);
                    _mcu_serial->setParity(QSerialPort::NoParity);
                    _mcu_serial->setStopBits(QSerialPort::OneStop);
                    _mcu_serial->setFlowControl(QSerialPort::NoFlowControl);
                    QObject::connect(_mcu_serial, SIGNAL(readyRead()), this, SLOT(readSerial()));
                    QObject::connect(&_serialTimeoutTimer, SIGNAL(timeout()), this, SLOT(onReadFinished()));
                    QObject::connect(&_sendQueuedSerialTimer, SIGNAL(timeout()), this, SLOT(writeQueuedSerial()));
                    break;
//                }
//            }
//        }
    }
    if(_mcu_serial->isOpen())
        qDebug() << _mcu_serial->portName() << " Detected!";
    else
        qDebug() << "Keyboard Device Not Found.";
}

void MainWindow::writeButtonToSerial(int buttonIndex)
{
    QByteArray command = Util::toByteArray(ButtonData::getButtonData(buttonIndex)->toJson());
    _mcu_serial->write(command.toStdString().c_str());
}

void MainWindow::writeQueuedSerial()
{
    qDebug() << "Length" << _queuedSerialToSend.length();
    if(_queuedSerialToSend.length() > 0)
    {
        qDebug() << "Sending from queue" << _queuedSerialToSend.at(0).length();
        _mcu_serial->write(_queuedSerialToSend.at(0).toStdString().c_str());
        _queuedSerialToSend.removeFirst();
        _sendQueuedSerialTimer.start(_SERIAL_WRITE_TIMEOUT);
    }
    else
    {
        QMessageBox::information(this, "Success", "All buttons written!");
        qDebug() << "Queue is finished";
    }
}

void MainWindow::readSerial()
{
    _serialTimeoutTimer.start(_SERIAL_READ_TIMEOUT);
    QByteArray receivedData = _mcu_serial->readAll();
    print("K: " + receivedData + '\n');
    if(receivedData.contains("Write was successful."))
    {
        print("Confirmation received");
        _sendQueuedSerialTimer.stop();
        writeQueuedSerial();
    }
}

void MainWindow::onReadFinished()
{
//    QMessageBox::information(this, "Serial received", _serialBuffer);
//    qDebug() << _serialBuffer << ++_debugInt;
    //If MCU acknowledged a successful write, send next button immediately
    _serialBuffer = "";
}

void MainWindow::on_actionExport_triggered()
{
    QJsonObject json;
    json.insert(BUTTONS, ButtonData::allButtonsToJson());
    Util::writeToFile(json, chooseJsonSavePath());
}


void MainWindow::on_actionImport_triggered()
{
    QJsonObject json = Util::parseJson(QFile(chooseJsonReadPath())).object();
    QJsonValue jsonButtonsData = json.value(BUTTONS);
    if(jsonButtonsData.isObject())
    {
        ButtonData::setButtonsData(ButtonData::buttonsDataFromJson(jsonButtonsData.toObject()));
        updateItemsTree();
    }
    else
        QMessageBox::warning(this, "Invalid JSON", "Invalid JSON structure of selected file.");
}


void MainWindow::on_actionShow_Console_enabledChanged(bool enabled)
{
    ui->outputTextBox->setVisible(enabled);
}

