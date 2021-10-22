#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "util.h"
#include "buttonData.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    ui->setupUi( this );

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
    _buttons.reserve(Prefs::bRow * Prefs::bCol);

    //Populate the buttons
    for( int row = 0; row < Prefs::bRow; ++row )
    {
        for( int col = 0; col < Prefs::bCol; ++col )
        {
            int index = row*Prefs::bCol + col;
            QPushButton *button = new QPushButton( "Button " + QString::number( index + 1 ) );
            button->setMinimumSize( _bSize, _bSize );
            button->setMaximumSize( _bSize, _bSize );

            connect(button, SIGNAL(clicked()), this, SLOT(buttonPress()));

            _buttons.emplace(index, button);
            ui->buttonLayout->addWidget( _buttons[ index ], row, col );
        }
    }
    selectButton( _selectedButtonIndex );

//    this->window()->adjustSize();
//    this->window()->setFixedSize( this->window()->size() );
}

MainWindow::~MainWindow()
{
    for (const auto &bt : _buttons)
        delete bt;

    delete ui;
}

void MainWindow::initItemTree()
{
    ui->buttonInfoTreeWidget->setAlternatingRowColors(true);
    ui->buttonInfoTreeWidget->setStyleSheet( "alternate-background-color: #dddddd" );

    ui->buttonInfoTreeWidget->header()->setVisible( true );
    ui->buttonInfoTreeWidget->header()->resizeSection( Prefs::numberRow, 10 );

    for(QString key : ButtonData::DATA_KEYS)
    {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem();
        treeItem->setText( Prefs::keyRow, key );
        treeItem->setFlags( Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable );
        ui->buttonInfoTreeWidget->addTopLevelItem( treeItem );
        treeItem->setExpanded( true );
        _treeItemsMap.insert(key, treeItem);
    }
}

void MainWindow::updateItemsTree()
{
    for(QString dataKey : ButtonData::DATA_KEYS)
    {
        QTreeWidgetItem *treeItem = _treeItemsMap.value(dataKey);
        Util::clearQTreeWidgetItemChildren( treeItem );
    }
    if( _selectedButtonIndex >= 0 )
    {
        ButtonData *data = getSelectedButtonData();
        #ifdef DEBUG
        qDebug() << "Loading button" << _selectedButtonIndex+1;
        #endif
        for(QString dataKey : ButtonData::DATA_KEYS)
        {
            int i = 0;
            QTreeWidgetItem *treeItem = _treeItemsMap.value(dataKey);

            for( Entry *item : *data->getData(dataKey) )
            {
                #ifdef DEBUG
                qDebug() << "\tLoading" << dataKey << item->type() << item->value() << '|' << item->properties()->size() << "properties";
                #endif
                treeItem->addChild( item->toTreeWidgetItem() );
            }
        }
    }
    updateItemsTreeIndices();
}

void MainWindow::updateItemsTreeIndices()
{
    if( _selectedButtonIndex >= 0 )
    {
        for(QString dataKey : ButtonData::DATA_KEYS)
        {
            QTreeWidgetItem *tree = _treeItemsMap.value(dataKey);
            for(int i = 0; i < tree->childCount(); i++)
                tree->child(i)->setText(Prefs::numberRow, QString::number(i+1));
        }
    }
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
        if( item->columnCount() > 1 )
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

ButtonData *MainWindow::getSelectedButtonData()
{
    return ButtonData::getButtonData(_selectedButtonIndex);
}

void MainWindow::buttonPress()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    const int index = _buttons.indexOf( button );

    selectButton( index );
}

void MainWindow::selectButton( int index )
{
    QPushButton *button = _buttons.value( _selectedButtonIndex );
    button->setStyleSheet( "" );
    ui->nameLabel->clear();

    _selectedButtonIndex = index;
    button = _buttons.value( _selectedButtonIndex );
    button->setStyleSheet( "background-color: green" );
    ui->nameLabel->setText( button->text() );

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

void MainWindow::on_buttonInfoTreeWidget_itemClicked( QTreeWidgetItem *item, int column )
{
    if( item->parent() && column == Prefs::valueRow )
        ui->buttonInfoTreeWidget->editItem( item, column );
}

void MainWindow::on_removeActionButton_clicked()
{
    QList<QTreeWidgetItem*> items = ui->buttonInfoTreeWidget->selectedItems();
    for( QTreeWidgetItem *item : items )
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

void MainWindow::on_buttonInfoTreeWidget_itemChanged( QTreeWidgetItem *item, int column )
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
    Util::writeToFile(*ButtonData::allButtonsToJson(), "./output.json");
}

