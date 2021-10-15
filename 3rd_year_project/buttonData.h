#ifndef ACTIONS_H
#define ACTIONS_H

#include "util.h"
#include "action.h"

#include <QMap>
#include <QString>

typedef QList<Entry*> EntryList;
typedef QMap<QString, EntryList*> EntryListMap;

class ButtonData
{
    public:
        ButtonData( EntryList actions = {} );

        static void init();
        static QList<QString> getActionsKeys();
        static Entry *getTemplateAction( QString type );

        static ButtonData *getButtonActions( int i );

        void addAction( Entry *action );

        void delAction( Entry *action );
        void delAction( int i );

        EntryList *getActions();
        Entry *getAction( int i );

    private:
        inline static QMap<QString, Entry*> _templateActions;
        inline static QMap<int, ButtonData*> _buttonsActionsLists;

        EntryListMap *_data = new EntryListMap();
        EntryList *_actions = new EntryList();

        static void addTemplateAction( QString key, QString value, Properties properties = {} );
};

#endif // ACTIONS_H
