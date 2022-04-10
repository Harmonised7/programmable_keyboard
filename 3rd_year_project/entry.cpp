#include "entry.h"
#include "mainwindow.h"

Entry::Entry( QString type, QString value, Properties properties ) :
    _type( type ),
    _value( value )
{
    for( const QString &key : properties.keys() )
    {
        _properties->insert(key, properties.value(key));
    }
}

Entry::Entry( QTreeWidgetItem *treeWidgetItem ) :
    _type( treeWidgetItem->text( 0 ) ),
    _value( treeWidgetItem->text( 1 ) )
{
    _properties = {};
    for( const QTreeWidgetItem *childTreeWidgetItem : treeWidgetItem->takeChildren() )
    {
        _properties->insert( childTreeWidgetItem->text(0), childTreeWidgetItem->text(1) );
    }
}

QTreeWidgetItem* Entry::toTreeWidgetItem()
{
    QTreeWidgetItem *actionItem = new QTreeWidgetItem();
    actionItem->setText( Prefs::keyRow, _type );
    actionItem->setText( Prefs::valueRow, _value );
    actionItem->setFlags( Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable );
    for( QString key : _properties->keys() )
    {
        QString value = _properties->value( key );
        QTreeWidgetItem *propertyItem = new QTreeWidgetItem;
        propertyItem->setText( Prefs::keyRow, key );
        propertyItem->setText( Prefs::valueRow, value );
        propertyItem->setFlags( Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable );
        actionItem->addChild( propertyItem );
    }
    return actionItem;
}

QJsonObject *Entry::toJson()
{
    QJsonObject *json = new QJsonObject;
    json->insert(ButtonData::getToAlias(TYPE), ButtonData::getToAlias(_type));
    //The other side assumes value is 0 - do not put 0 if it's 0.
    if
        (_value.length() > 1 ||
        (_value.length() > 0 && _value.at(0) != '0'))
    {
        QString value = _value;
        if(_type == "key" || _type == "key_down" || _type == "key_up")
        {
            int asciiValue = ButtonData::getToAsciiAlias(_value);
            json->insert(ButtonData::getToAlias(VALUE), asciiValue == 0 ? _value.at(0).toLatin1() : asciiValue);
        }
        else
            json->insert(ButtonData::getToAlias(VALUE), value);
    }
    QString outValue = "";
    int length = _value.length();
    for(int i = 0; i < length; ++i)
    {
        QChar c = _value.at(i);
        if(c != '\\')
        {
            outValue.append(c);
            continue;
        }
        else if(i+1 < length)
        {
            QChar nextChar = _value.at(i+1);
            //Handle \n and \t
            if(nextChar == 'n')
            {
                qDebug() << outValue;
                outValue.append('\n');
                qDebug() << outValue;
                ++i;
            }
            else if(nextChar == 't')
            {
                outValue.append('\t');
                ++i;
            }
        }
    }
    for(QString key : _properties->keys())
        json->insert(ButtonData::getToAlias(key), _properties->value(key));
    return json;
}

void Entry::setProperty(const QString &property, const QString &value)
{
    if( !_properties->contains(property) )
        qDebug() << "WARNING: " << _type << "did not have key" << property;
    if( property != "text" && property != "key" )
        _properties->insert(property, QString::number(value.toDouble()));
    else
        _properties->insert(property, value);
}

Properties *Entry::properties()
{
    return _properties;
}
