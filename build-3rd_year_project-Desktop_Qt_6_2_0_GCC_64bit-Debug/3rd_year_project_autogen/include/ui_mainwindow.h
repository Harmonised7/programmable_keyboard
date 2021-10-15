/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *mainHorizontalLayout;
    QGridLayout *buttonLayout;
    QVBoxLayout *buttonInfoVerticalLayout;
    QLabel *nameLabel;
    QHBoxLayout *addActionHorizontalLayout;
    QComboBox *actionTypeBox;
    QPushButton *addActionButton;
    QHBoxLayout *buttonInfoHorizontalLayout;
    QTreeWidget *buttonInfoTreeWidget;
    QVBoxLayout *buttonInfoTreeButtonsVerticalLayout;
    QSpacerItem *verticalSpacer_2;
    QPushButton *moveItemUpButton;
    QPushButton *removeActionButton;
    QPushButton *moveItemDownButton;
    QSpacerItem *verticalSpacer;
    QPushButton *updateButton;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(960, 540);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mainHorizontalLayout = new QHBoxLayout();
        mainHorizontalLayout->setObjectName(QString::fromUtf8("mainHorizontalLayout"));
        buttonLayout = new QGridLayout();
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));

        mainHorizontalLayout->addLayout(buttonLayout);

        buttonInfoVerticalLayout = new QVBoxLayout();
        buttonInfoVerticalLayout->setObjectName(QString::fromUtf8("buttonInfoVerticalLayout"));
        nameLabel = new QLabel(centralwidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        buttonInfoVerticalLayout->addWidget(nameLabel);

        addActionHorizontalLayout = new QHBoxLayout();
        addActionHorizontalLayout->setObjectName(QString::fromUtf8("addActionHorizontalLayout"));
        actionTypeBox = new QComboBox(centralwidget);
        actionTypeBox->setObjectName(QString::fromUtf8("actionTypeBox"));

        addActionHorizontalLayout->addWidget(actionTypeBox);

        addActionButton = new QPushButton(centralwidget);
        addActionButton->setObjectName(QString::fromUtf8("addActionButton"));
        addActionButton->setMaximumSize(QSize(25, 25));

        addActionHorizontalLayout->addWidget(addActionButton);


        buttonInfoVerticalLayout->addLayout(addActionHorizontalLayout);

        buttonInfoHorizontalLayout = new QHBoxLayout();
        buttonInfoHorizontalLayout->setObjectName(QString::fromUtf8("buttonInfoHorizontalLayout"));
        buttonInfoTreeWidget = new QTreeWidget(centralwidget);
        QFont font;
        font.setBold(false);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setFont(1, font);
        buttonInfoTreeWidget->setHeaderItem(__qtreewidgetitem);
        buttonInfoTreeWidget->setObjectName(QString::fromUtf8("buttonInfoTreeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonInfoTreeWidget->sizePolicy().hasHeightForWidth());
        buttonInfoTreeWidget->setSizePolicy(sizePolicy);
        buttonInfoTreeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        buttonInfoTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        buttonInfoTreeWidget->setUniformRowHeights(false);
        buttonInfoTreeWidget->header()->setVisible(false);
        buttonInfoTreeWidget->header()->setCascadingSectionResizes(false);
        buttonInfoTreeWidget->header()->setMinimumSectionSize(28);
        buttonInfoTreeWidget->header()->setDefaultSectionSize(150);
        buttonInfoTreeWidget->header()->setProperty("showSortIndicator", QVariant(false));
        buttonInfoTreeWidget->header()->setStretchLastSection(true);

        buttonInfoHorizontalLayout->addWidget(buttonInfoTreeWidget);

        buttonInfoTreeButtonsVerticalLayout = new QVBoxLayout();
        buttonInfoTreeButtonsVerticalLayout->setObjectName(QString::fromUtf8("buttonInfoTreeButtonsVerticalLayout"));
        buttonInfoTreeButtonsVerticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        buttonInfoTreeButtonsVerticalLayout->addItem(verticalSpacer_2);

        moveItemUpButton = new QPushButton(centralwidget);
        moveItemUpButton->setObjectName(QString::fromUtf8("moveItemUpButton"));
        moveItemUpButton->setMaximumSize(QSize(20, 16777215));

        buttonInfoTreeButtonsVerticalLayout->addWidget(moveItemUpButton);

        removeActionButton = new QPushButton(centralwidget);
        removeActionButton->setObjectName(QString::fromUtf8("removeActionButton"));
        removeActionButton->setMaximumSize(QSize(20, 16777215));

        buttonInfoTreeButtonsVerticalLayout->addWidget(removeActionButton);

        moveItemDownButton = new QPushButton(centralwidget);
        moveItemDownButton->setObjectName(QString::fromUtf8("moveItemDownButton"));
        moveItemDownButton->setMaximumSize(QSize(20, 16777215));

        buttonInfoTreeButtonsVerticalLayout->addWidget(moveItemDownButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        buttonInfoTreeButtonsVerticalLayout->addItem(verticalSpacer);


        buttonInfoHorizontalLayout->addLayout(buttonInfoTreeButtonsVerticalLayout);


        buttonInfoVerticalLayout->addLayout(buttonInfoHorizontalLayout);

        updateButton = new QPushButton(centralwidget);
        updateButton->setObjectName(QString::fromUtf8("updateButton"));

        buttonInfoVerticalLayout->addWidget(updateButton);


        mainHorizontalLayout->addLayout(buttonInfoVerticalLayout);


        verticalLayout->addLayout(mainHorizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 960, 22));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Blue face for an Interface", nullptr));
        nameLabel->setText(QCoreApplication::translate("MainWindow", "nameLabel", nullptr));
        addActionButton->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = buttonInfoTreeWidget->headerItem();
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "#", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Value", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Key", nullptr));
        moveItemUpButton->setText(QCoreApplication::translate("MainWindow", "/\\", nullptr));
        removeActionButton->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        moveItemDownButton->setText(QCoreApplication::translate("MainWindow", "\\/", nullptr));
        updateButton->setText(QCoreApplication::translate("MainWindow", "Update", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
