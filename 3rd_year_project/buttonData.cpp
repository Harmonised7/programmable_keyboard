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

    addButtonTemplateActions(ButtonData::PRESS);
    addButtonTemplateActions(ButtonData::HOLD);
    addButtonTemplateActions(ButtonData::RELEASE);
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

QJsonArray *ButtonData::allButtonsToJson()
{
    QJsonArray *json = new QJsonArray;
    for(int index : _buttonsDataMap->keys())
        json->push_back(getButtonData(index)->toJson());
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
    for(QString dataKey : DATA_KEYS)
    {
        QJsonArray dataJsonArray;
        for(Entry *entry : *getData(dataKey))
            dataJsonArray.push_back(*entry->toJson());
        json.insert(dataKey, dataJsonArray);
    }
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

    addTemplateAction( templateType, "type", "a sentence", {{ "speed", "20" }} );

    addTemplateAction( templateType, "press", "ENTER", {{ "duration", "50" }} );
    addTemplateAction( templateType, "press_down", "ENTER" );
    addTemplateAction( templateType, "press_up", "ENTER" );

    addTemplateAction( templateType, "click", "0", { { "times", "5" }, { "speed", "20" } } );
    addTemplateAction( templateType, "click_up", "0" );
    addTemplateAction( templateType, "click_down", "0" );

    addTemplateAction( templateType, "move_mouse", "0", { { "x", "100" }, { "y", "100" }, { "speed", "20" } } );
    addTemplateAction( templateType, "drag_mouse", "0", { { "x", "100" }, { "y", "100" }, { "speed", "20" } } );
    addTemplateAction( templateType, "set_mouse", "0", { { "x", "100" }, { "y", "100" } } );
}

ButtonsDataMap ButtonData::buttonsDataFromJson(QJsonArray jsonButtonsData)
{
    ButtonsDataMap buttonsData;
    int buttonIndex = 0;
    for(const QJsonValueRef jsonButtonDataRef : jsonButtonsData)
    {
        QJsonObject jsonButtonData = jsonButtonDataRef.toObject();
        ButtonData *buttonData = new ButtonData;
        for(const QString dataKey : jsonButtonData.keys())
        {
            QJsonArray jsonEntriesList = jsonButtonData.value(dataKey).toArray();
            for(const QJsonValueRef jsonEntryRef : jsonEntriesList)
            {
                QJsonObject jsonEntry = jsonEntryRef.toObject();
                QString type = jsonEntry.value(Entry::TYPE).toString();
                QString value = jsonEntry.value(Entry::VALUE).toString();
                Entry *entry = ButtonData::generateTemplateEntry(dataKey, type);
                entry->setValue(value);
                QJsonObject jsonProperties = jsonEntry.value(Entry::PROPERTIES).toObject();
                for(const QString propertyKey : jsonProperties.keys())
                    entry->setProperty(propertyKey, jsonProperties.value(propertyKey).toString());
                buttonData->addEntry(dataKey, entry);
            }
        }
        buttonsData.insert(buttonIndex, buttonData);
        ++buttonIndex;
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
    int buttonCount = Prefs::bCol * Prefs::bRow;
    for(int i = 0; i < buttonCount; ++i)
    {
        _buttonsDataMap->insert(i, new ButtonData());
    }
}
