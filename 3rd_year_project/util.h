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

    static void writeToFile(QJsonArray json, QString outputPath = "./")
    {
        writeToFile(QJsonDocument(json), outputPath);
    }

    static void writeToFile(QJsonObject json, QString outputPath = "./")
    {
        writeToFile(QJsonDocument(json), outputPath);
    }

    static void writeToFile(QJsonDocument json, QString outputPath = "./")
    {
        writeToFile(json.toJson(), outputPath);
    }

    static void writeToFile(QByteArray byteArray, QString outputPath = "./")
    {
        if(!outputPath.isEmpty())
        {
            QFile output(outputPath);
            if(output.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                output.write(byteArray);
                output.close();
            }
        }
    }

    static QString chooseFileDir(QString startPath = "./")
    {
        return QFileDialog::getSaveFileName(nullptr, "nowo", startPath, jsonFilter, &jsonFilter, QFileDialog::DontUseNativeDialog);
    }

    inline static QString jsonFilter = "JSON (*.json)";

    private:
};

#endif // UTIL_H
