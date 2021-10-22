#ifndef ACTION_H
#define ACTION_H

#include <QTreeWidgetItem>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

typedef QMap<QString, QString> Properties;

class Entry
{
    public:
        Entry( QString type, QString value, Properties properties = {} );
        Entry( QTreeWidgetItem *treeWidgetItem );

        QString type(){return _type;};
        QString value(){return _value;};
        QString property(const QString key){return _properties->value(key);};

        void setType(const QString &newType){_type = newType;};
        void setValue(const QString &newValue){_value = newValue;};
        void setOpen(const bool open){_open = open;};
        void setProperty(const QString &property, const QString &newValue);

        Properties *properties();

        QTreeWidgetItem *toTreeWidgetItem();

        QJsonObject *toJson();

        //Static fields
        inline static const QString TYPE = "type";
        inline static const QString VALUE = "value";
        inline static const QString PROPERTIES = "properties";
private:
        QString _type;
        QString _value;
        Properties *_properties = new Properties;
        bool _open;
};

#endif // ACTION_H
