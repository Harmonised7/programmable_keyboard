#include "buttonData.h"

ButtonData::ButtonData( EntryListMap entryListMap )
{
    for(QString key : ButtonData::DATA_KEYS)
    {
        if(entryListMap.contains(key))
            _data->insert(key, entryListMap.value(key));
        else
            _data->insert(key, new EntryList());
    }
}
/**
 * static initializer
 */
void ButtonData::init()
{
    wipeButtonsData();
    for(QString key : ButtonData::DATA_KEYS)
        _templateEntriesMap->insert(key, new TemplateEntries);

//    addTemplateAction(ButtonData::MISC, "red", "255");
//    addTemplateAction(ButtonData::MISC, "green", "255");
//    addTemplateAction(ButtonData::MISC, "blue", "255");

    registerAliases();
    addButtonTemplateActions(ButtonData::PRESS);
//    addButtonTemplateActions(ButtonData::HOLD);
//    addButtonTemplateActions(ButtonData::RELEASE);
}



QList<QString> ButtonData::getTemplateKeys(QString dataType)
{
    return _templateEntriesMap->value(dataType)->keys();
}

Entry *ButtonData::generateTemplateEntry(QString dataType, QString type)
{
    Entry *entry = _templateEntriesMap->value(dataType)->value(type);
    Entry *newAction = new Entry( entry->type(), entry->value());
    for(const QString key : entry->properties()->keys())
    {
        newAction->setProperty(key, entry->property(key));
    }
    return newAction;
}

bool ButtonData::hasTemplate(QString dataType, QString type)
{
    return _templateEntriesMap->value(dataType)->contains(type);
}

ButtonData *ButtonData::getButtonData(int buttonIndex)
{
    return _buttonsDataMap->value(buttonIndex);
}

QJsonObject ButtonData::allButtonsToJson()
{
    QJsonObject json;
    for(int index : _buttonsDataMap->keys())
    {
        QJsonObject buttonJson = getButtonData(index)->toJson();
        if(buttonJson.value("a").toArray().size() > 0)
            json.insert(QString::number(index), buttonJson);
    }
    return json;
}

EntryList *ButtonData::getData(QString dataType)
{
    return _data->value(dataType);
}

void ButtonData::addEntry(QString dataType, Entry *entry)
{
    getEntries(dataType)->push_back(entry);
}

void ButtonData::delEntry(QString dataType, Entry *entry)
{
    delEntry(dataType, getEntries(dataType)->indexOf(entry));
}

void ButtonData::delEntry(QString dataType, int entryIndex)
{
    getEntries(dataType)->remove(entryIndex);
}

EntryList *ButtonData::getEntries(QString dataType)
{
    return _data->value(dataType);
}

Entry *ButtonData::getEntry(QString dataType, int entryIndex)
{
    return getEntries(dataType)->value(entryIndex);
}

QJsonObject ButtonData::toJson()
{
    QJsonObject json;
//    for(QString dataKey : DATA_KEYS)
//    {
//        QJsonArray dataJsonArray;
//        for(Entry *entry : *getData(dataKey))
//            dataJsonArray.push_back(*entry->toJson());
//        json.insert(dataKey, dataJsonArray);
//    }
    QJsonArray dataJsonArray;
    for(Entry *entry : *getData(PRESS))
        dataJsonArray.push_back(*entry->toJson());
    json.insert("a", dataJsonArray);
    return json;
}

//ButtonData ButtonData::fromJson(QJsonObject &jsonButtonData)
//{
//    return
//}

void ButtonData::addTemplateAction(QString templateType, QString key, QString value, Properties properties)
{
    _templateEntriesMap->value(templateType)->insert(key, new Entry(key, value, properties));
}

void ButtonData::addButtonTemplateActions(QString templateType)
{
    addTemplateAction( templateType, "delay", "500" );

    addTemplateAction( templateType, "write", "a sentence", {{ "delay", "20" }} );

    addTemplateAction( templateType, "press", "ENTER", {{ "duration", "50" }} );
    addTemplateAction( templateType, "press_down", "ENTER" );
    addTemplateAction( templateType, "press_up", "ENTER" );

    addTemplateAction( templateType, "click", "0", { { "times", "5" }, { "delay", "20" } } );
    addTemplateAction( templateType, "click_up", "0" );
    addTemplateAction( templateType, "click_down", "0" );

    addTemplateAction( templateType, "move_mouse", "0", { { "x", "100" }, { "y", "100" }, { "delay", "20" } } );
    addTemplateAction( templateType, "drag_mouse", "0", { { "x", "100" }, { "y", "100" }, { "delay", "20" } } );
    addTemplateAction( templateType, "set_mouse", "0", { { "x", "100" }, { "y", "100" } } );
}

void ButtonData::registerAliases()
{
    registerAlias("type", "t");
    registerAlias("write", "w");
    registerAlias("click", "c");
    registerAlias("delay", "d");
    registerAlias("value", "v");
    registerAlias("times", "x");
}

void ButtonData::registerAlias(QString a, QString b)
{
    _aliasTo->insert(a, b);
    _aliasFrom->insert(b, a);
}

QString ButtonData::getToAlias(QString word)
{
    return _aliasTo->contains(word) ? _aliasTo->value(word) : word;
}

QString ButtonData::getFromAlias(QString word)
{
    return _aliasFrom->contains(word) ? _aliasFrom->value(word) : word;
}

ButtonsDataMap ButtonData::buttonsDataFromJson(QJsonObject jsonButtonsData)
{
    QString TYPE_ALIAS = ButtonData::getToAlias(Entry::TYPE);
    QString VALUE_ALIAS = ButtonData::getToAlias(Entry::VALUE);
    ButtonsDataMap buttonsData;
    for(int i = 0; i < Prefs::bCount; ++i)
    {
        if(!jsonButtonsData.contains(QString::number(i)))
            continue;
        QJsonObject jsonEntry = jsonButtonsData.value(QString::number(i)).toObject();
        if(!jsonEntry.contains("a"))
            continue;
        ButtonData *buttonData = new ButtonData;
        for(QJsonValueRef jsonActionRef : jsonEntry.value("a").toArray())
        {
            QJsonObject jsonAction = jsonActionRef.toObject();
            if(!jsonAction.contains(TYPE_ALIAS))
                continue;
            QString type = ButtonData::getFromAlias(jsonAction.value(TYPE_ALIAS).toString());
            QString value = jsonAction.contains(VALUE_ALIAS) ? jsonAction.value(VALUE_ALIAS).toString() : 0;
            Entry *entry = ButtonData::generateTemplateEntry(PRESS, type);
            entry->setValue(value);
            for(const QString propertyAlias : jsonAction.keys())
            {
                QString property = ButtonData::getFromAlias(propertyAlias);
                if(property != Entry::TYPE && property != Entry::VALUE)
                    entry->setProperty(property, jsonAction.value(propertyAlias).toString());
            }
            buttonData->addEntry(PRESS, entry);
        }
        buttonsData.insert(i, buttonData);
    }
    return buttonsData;
}

void ButtonData::setButtonData(int buttonIndex, ButtonData *data)
{
    _buttonsDataMap->insert(buttonIndex, data);
}

void ButtonData::setButtonsData(ButtonsDataMap buttonsData)
{
    int i = 0;
    for(ButtonData *buttonData : buttonsData)
        setButtonData(i++, buttonData);
}

void ButtonData::wipeButtonsData()
{
    delete _buttonsDataMap;
    _buttonsDataMap = new ButtonsDataMap;
    for(int i = 0; i < Prefs::bCount; ++i)
    {
        _buttonsDataMap->insert(i, new ButtonData());
    }
}
