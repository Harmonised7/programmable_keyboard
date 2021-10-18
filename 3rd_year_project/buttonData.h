#ifndef ACTIONS_H
#define ACTIONS_H

#include "util.h"
#include "action.h"

#include <QMap>
#include <QString>

typedef QList<Entry*> EntryList;
typedef QMap<QString, EntryList*> EntryListMap;

typedef QMap<QString, Entry*> TemplateEntries;
typedef QMap<QString, TemplateEntries*> TemplateEntriesMap;

class ButtonData
{
    public:
        ButtonData( EntryList actions = {} );

        static void init();
        static QList<QString> getTemplateKeys(QString dataType);
        static Entry *generateTemplateEntry(QString dataType, QString type);
        static bool hasTemplate(QString dataType, QString type);

        static ButtonData *getButtonData(int buttonIndex);

        EntryList *getData(QString dataType);

        void addEntry(QString dataType, Entry *action);

        void delEntry(QString dataType, Entry *action);
        void delEntry(QString dataType, int entryIndex);

        EntryList *getEntries(QString dataType);
        Entry *getEntry(QString dataType,  int entryIndex);

//        inline static const QString MISC = "misc";
        inline static const QString PRESS = "press";
        inline static const QString HOLD = "hold";
        inline static const QString RELEASE = "release";
        inline static const QList<QString> DATA_KEYS = {/*MISC,*/ PRESS, HOLD, RELEASE};

    private:
        inline static TemplateEntriesMap *_templateEntriesMap = new TemplateEntriesMap;
        inline static QMap<int, ButtonData*> *_buttonsDataMap = new QMap<int, ButtonData*>;

        EntryListMap *_data = new EntryListMap();
//        EntryList *_actions = new EntryList();

        static void addTemplateAction(QString templateType, QString key, QString value, Properties properties = {});
        static void addButtonTemplateActions(QString templateType);
};

#endif // ACTIONS_H
