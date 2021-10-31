#ifndef UTIL_H
#define UTIL_H

#include "entry.h"
#include "prefs.h"

#include <QMap>
#include <QDebug>
#include <QList>
#include <QString>
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

class Util
{
public:
    static void clearQTreeWidgetItemChildren( QTreeWidgetItem *item )
    {
        for( QTreeWidgetItem *child : item->takeChildren() )
        {
            item->removeChild( child );
        }
    }

    static bool writeToFile(QJsonArray json, QString outputPath)
    {
        return writeToFile(QJsonDocument(json), outputPath);
    }

    static bool writeToFile(QJsonObject json, QString outputPath)
    {
        return writeToFile(QJsonDocument(json), outputPath);
    }

    static bool writeToFile(QJsonDocument json, QString outputPath)
    {
        return writeToFile(json.toJson(), outputPath);
    }

    static bool writeToFile(QByteArray byteArray, QString outputPath)
    {
        QFile output(outputPath);
        if(output.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            output.write(byteArray);
            output.close();
            return true;
        }
        else
            QMessageBox::critical(nullptr, "File in use", output.errorString());
        return false;
    }

    static QJsonDocument parseJson(QFile file)
    {
        if(file.exists())
        {
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString jsonString = file.readAll();
                file.close();
                return QJsonDocument::fromJson(jsonString.toUtf8());
            }
        }
        return QJsonDocument();
    }

    private:
};

#endif // UTIL_H
