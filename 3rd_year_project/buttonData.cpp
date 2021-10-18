#include "buttonData.h"

ButtonData::ButtonData( QList<Entry *> actions )
{
    for(QString key : ButtonData::DATA_KEYS)
    {
        _data->insert(key, new EntryList());
    }
    for(Entry *action : actions)
    {
        _data->value(ButtonData::PRESS)->push_back(action);
    }
}
/**
 * static initializer
 */
void ButtonData::init()
{
    int buttonCount = Prefs::bCol * Prefs::bRow;

    for(QString key : ButtonData::DATA_KEYS)
        _templateEntriesMap->insert(key, new TemplateEntries);
    for( int i = 0; i < buttonCount; ++i )
    {
        _buttonsDataMap->insert( i, new ButtonData() );
    }
    addTemplateAction(ButtonData::MISC, "red", "255");
    addTemplateAction(ButtonData::MISC, "green", "255");
    addTemplateAction(ButtonData::MISC, "blue", "255");

    addButtonTemplateActions(ButtonData::PRESS);
    addButtonTemplateActions(ButtonData::HOLD);
    addButtonTemplateActions(ButtonData::RELEASE);
}



QList<QString> ButtonData::getTemplateKeys(QString dataType)
{
    return _templateEntriesMap->value(dataType)->keys();
}

Entry *ButtonData::getTemplateEntry(QString dataType, QString type)
{
//    if( !_templateActions.contains( type ) )
//        qDebug() << "Invalid action type '" << type << "' requested!";
    Entry *action = _templateEntriesMap->value(dataType)->value( type );
    Entry *newAction = new Entry( action->type(), action->value() );
    for( const QString key : action->properties()->keys() )
    {
        newAction->setProperty( key, action->property(key) );
    }
    return newAction;
}

ButtonData *ButtonData::getButtonData(int buttonIndex)
{
    return _buttonsDataMap->value(buttonIndex);
}

EntryList *ButtonData::getData(QString dataType)
{
    return _data->value(dataType);
}

void ButtonData::addEntry(QString dataType, Entry *action)
{
    getEntries(dataType)->push_back( action );
}

void ButtonData::delEntry(QString dataType, Entry *action)
{
    delEntry(dataType, getEntries(dataType)->indexOf(action));
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
