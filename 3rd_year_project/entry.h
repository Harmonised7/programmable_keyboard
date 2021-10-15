#ifndef ACTION_H
#define ACTION_H

#include <QTreeWidgetItem>
#include <QMap>
#include <QString>

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
        //TODO: toJson

private:
        QString _type;
        QString _value;
        Properties *_properties = new Properties;
        bool _open;
};

#endif // ACTION_H
