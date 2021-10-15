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

    void updateActionsTree();
    int getActionIndex( QTreeWidgetItem *item );
    bool isChildOfActions( QTreeWidgetItem *item );
    ButtonData *getSelectedActions();

public slots:
    void buttonPress();
    void selectButton( int index );

private slots:
    void on_addActionButton_clicked();

    void on_buttonInfoTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_removeActionButton_clicked();

    void on_buttonInfoTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;

    // Objects
    QList<QPushButton*> _buttons;
    const float _bSize = 100;
    int _selectedButtonIndex = 0;
    QTreeWidgetItem *_actionsTreeItem;

    // Methods
    // ...
};
#endif // MAINWINDOW_H
