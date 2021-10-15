#include "buttonData.h"

ButtonData::ButtonData( QList<Entry *> actions )
{
    for( Entry *action : actions )
    {
        _actions->push_back( action );
    }
}

void ButtonData::init()
{
    int buttonCount = Prefs::bCol * Prefs::bRow;
    for( int i = 0; i < buttonCount; ++i )
    {
        _buttonsActionsLists.insert( i, new ButtonData() );
    }

    addTemplateAction( "delay", "500" );

    addTemplateAction( "type", "a sentence", {{ "speed", "20" }} );

    addTemplateAction( "press", "ENTER", {{ "duration", "50" }} );
    addTemplateAction( "press_down", "ENTER" );
    addTemplateAction( "press_up", "ENTER" );

    addTemplateAction( "click", "0", { { "times", "5" }, { "speed", "20" } } );
    addTemplateAction( "click_up", "0" );
    addTemplateAction( "click_down", "0" );

    addTemplateAction( "move_mouse", "0", { { "x", "100" }, { "y", "100" }, { "speed", "20" } } );
    addTemplateAction( "drag_mouse", "0", { { "x", "100" }, { "y", "100" }, { "speed", "20" } } );
    addTemplateAction( "set_mouse", "0", { { "x", "100" }, { "y", "100" } } );
}



QList<QString> ButtonData::getActionsKeys()
{
    return _templateActions.keys();
}

Entry *ButtonData::getTemplateAction( QString type )
{
    if( !_templateActions.contains( type ) )
        qDebug() << "Invalid action type '" << type << "' requested!";
    Entry *action = _templateActions.value( type );
    Entry *newAction = new Entry( action->type(), action->value() );
    for( const QString key : action->properties()->keys() )
    {
        newAction->setProperty( key, action->property(key) );
    }
    return newAction;
}

ButtonData *ButtonData::getButtonActions( int i )
{
    return _buttonsActionsLists.value( i );
}

void ButtonData::addAction( Entry *action )
{
    _actions->push_back( action );
}

void ButtonData::delAction( Entry *action )
{
    delAction( _actions->indexOf( action ) );
}

void ButtonData::delAction( int i )
{
    _actions->remove( i );
}

QList<Entry *> *ButtonData::getActions()
{
    return _actions;
}

Entry *ButtonData::getAction( int i )
{
    return _actions->value( i );
}

void ButtonData::addTemplateAction( QString key, QString value, Properties properties )
{
    _templateActions.insert( key, new Entry( key, value, properties ) );
}
