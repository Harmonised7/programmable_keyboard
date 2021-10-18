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

            connect(button, SIGNAL( clicked() ), this, SLOT( buttonPress() ));

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
    ui->buttonInfoTreeWidget->setStyleSheet( "alternate-background-color: #cccccc" );

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

void MainWindow::updateActionsTree()
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
            QTreeWidgetItem *treeItem = _treeItemsMap.value(dataKey);

            for( Entry *item : *data->getData(dataKey) )
            {
                #ifdef DEBUG
                qDebug() << "\tLoading action" << item->type() << item->value() << '|' << item->properties()->size() << "properties";
                #endif
                treeItem->addChild( item->toTreeWidgetItem() );
            }
        }
    }
}

QString MainWindow::getEntryType(QTreeWidgetItem *item)
{
    for(QString key : _treeItemsMap.keys())
    {
        if(isParentOf(item, _treeItemsMap.value(key)))
            return key;
    }
    qDebug() << "PARENTLESS TREE ITEM";
    return "";
}

int MainWindow::getEntryIndex(QTreeWidgetItem *item)
{

    //Actions can only be in second column
    if( item->columnCount() > 1 )
        return _treeItemsMap.value(getEntryType(item))->indexOfChild(item);
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
    QPushButton *button = qobject_cast<QPushButton *>( sender() );
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

    updateActionsTree();
}

void MainWindow::on_addActionButton_clicked()
{
    QString type = ui->entryTypeBox->currentText();
    ButtonData *data = getSelectedButtonData();
//    Entry *action = ButtonData::getTemplateEntry(type);
//    #ifdef DEBUG
//    qDebug() << "Adding Action" << type << "to button" << _selectedButtonIndex;
//    qDebug() << getSelectedButtonData()->getEntries()->size();
//    #endif
//    getSelectedButtonData()->addEntry( action );
//    #ifdef DEBUG
//    qDebug() << getSelectedButtonData()->getEntries()->size();
//    #endif
    updateActionsTree();
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
        int index = getEntryIndex( item );
        if( index >= 0 )
        {
            #ifdef DEBUG
            qDebug() << "Deleting action" << index << "in" << _selectedButtonIndex;
            #endif
//            getSelectedButtonData()->delEntry( index );
//            _actionsTreeItem->removeChild( item );
        }
    }
}

void MainWindow::on_buttonInfoTreeWidget_itemChanged( QTreeWidgetItem *item, int column )
{
//    int index = getEntryIndex(item);
//    Entry *action = getSelectedButtonData()->getEntry(index);
//    QString property = item->text(0);
//    QString value = item->text(1);
//    #ifdef DEBUG
//    qDebug() << "changed" << property << "to" << value;
//    #endif
//    if( isChildOfActions(item) )
//        action->setValue(value);
//    else
//        action->setProperty(property, value);
}

void MainWindow::on_dataTypeBox_currentIndexChanged(int index)
{
    qDebug() << index << ui->dataTypeBox->itemText(index);
    ui->entryTypeBox->clear();
    ui->entryTypeBox->addItems(ButtonData::getTemplateKeys(ui->dataTypeBox->itemText(index)));
}

