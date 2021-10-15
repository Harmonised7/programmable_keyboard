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

    //Populate action types
    for( QString type : ButtonData::getActionsKeys() )
    {
        ui->actionTypeBox->addItem( type );
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

    _actionsTreeItem = new QTreeWidgetItem();
    _actionsTreeItem->setText( Prefs::keyRow, "actions" );
    _actionsTreeItem->setFlags( Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable );
    ui->buttonInfoTreeWidget->addTopLevelItem( _actionsTreeItem );
    _actionsTreeItem->setExpanded( true );
}

void MainWindow::updateActionsTree()
{
    Util::clearQTreeWidgetItemChildren( _actionsTreeItem );
    if( _selectedButtonIndex >= 0 )
    {
        #ifdef DEBUG
        qDebug() << "Loading button" << _selectedButtonIndex+1 << '|' << getSelectedActions()->getActions()->size() << "actions";
        #endif
        for( Entry *item : *getSelectedActions()->getActions() )
        {
            #ifdef DEBUG
            qDebug() << "\tLoading action" << item->type() << item->value() << '|' << item->properties()->size() << "properties";
            #endif
            _actionsTreeItem->addChild( item->toTreeWidgetItem() );
        }
    }
}

int MainWindow::getActionIndex( QTreeWidgetItem *item )
{
    //Actions can only be in second column
    if( item->columnCount() > 1 )
    {
        if( item->parent() == _actionsTreeItem )
            return _actionsTreeItem->indexOfChild( item );
        else if( item->parent()->parent() == _actionsTreeItem )
            return _actionsTreeItem->indexOfChild( item->parent() );
    }
    return -1;
}

bool MainWindow::isChildOfActions(QTreeWidgetItem *item)
{
    return item->parent() == _actionsTreeItem;
}

ButtonData *MainWindow::getSelectedActions()
{
    return ButtonData::getButtonActions(_selectedButtonIndex);
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
    QString type = ui->actionTypeBox->currentText();
    Entry *action = ButtonData::getTemplateAction( type );
    #ifdef DEBUG
    qDebug() << "Adding Action" << type << "to button" << _selectedButtonIndex;
    qDebug() << getSelectedActions()->getActions()->size();
    #endif
    getSelectedActions()->addAction( action );
    #ifdef DEBUG
    qDebug() << getSelectedActions()->getActions()->size();
    #endif
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
        int index = getActionIndex( item );
        if( index >= 0 )
        {
            #ifdef DEBUG
            qDebug() << "Deleting action" << index << "in" << _selectedButtonIndex;
            #endif
            getSelectedActions()->delAction( index );
            _actionsTreeItem->removeChild( item );
        }
    }
}

void MainWindow::on_buttonInfoTreeWidget_itemChanged( QTreeWidgetItem *item, int column )
{
    int index = getActionIndex(item);
    Entry *action = getSelectedActions()->getAction(index);
    QString property = item->text(0);
    QString value = item->text(1);
    #ifdef DEBUG
    qDebug() << "changed" << property << "to" << value;
    #endif
    if( isChildOfActions(item) )
        action->setValue(value);
    else
        action->setProperty(property, value);
}
