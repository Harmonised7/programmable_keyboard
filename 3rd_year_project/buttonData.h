#ifndef ACTIONS_H
#define ACTIONS_H

#include "util.h"
#include "entry.h"

#include <QMap>
#include <QString>

typedef QList<Entry*> EntryList;
typedef QMap<QString, EntryList*> EntryListMap;

typedef QMap<QString, Entry*> TemplateEntries;
typedef QMap<QString, TemplateEntries*> TemplateEntriesMap;

class ButtonData
{
    public:
        ButtonData( EntryListMap actions = {} );

        //Static methods
        static void init();
        static QList<QString> getTemplateKeys(QString dataType);
        static Entry *generateTemplateEntry(QString dataType, QString type);
        static bool hasTemplate(QString dataType, QString type);
        static ButtonData *getButtonData(int buttonIndex);
        static QJsonObject allButtonsToJson();
        static QMap<int, ButtonData*> buttonsDataFromJson(QJsonArray jsonArray);
        static void setButtonData(int buttonIndex, ButtonData *data);
        static void setButtonsData(QMap<int, ButtonData*> buttonsData);
        static void wipeButtonsData();

        //Methods
        EntryList *getData(QString dataType);

        void addEntry(QString dataType, Entry *entry);

        void delEntry(QString dataType, Entry *entry);
        void delEntry(QString dataType, int entryIndex);

        EntryList *getEntries(QString dataType);
        Entry *getEntry(QString dataType,  int entryIndex);

        QJsonObject toJson();
        ButtonData fromJson(QJsonObject &jsonButtonData);

        //Static fields
//        inline static const QString MISC = "misc";
        inline static const QString PRESS = "p";
        inline static const QString HOLD = "h";
        inline static const QString RELEASE = "r";
        inline static const QList<QString> DATA_KEYS = {/*MISC,*/ PRESS, HOLD, RELEASE};

    private:
        //Static fields
        inline static TemplateEntriesMap *_templateEntriesMap = new TemplateEntriesMap;
        inline static QMap<int, ButtonData*> *_buttonsDataMap = new QMap<int, ButtonData*>;

        //Fields
        EntryListMap *_data = new EntryListMap();
//        EntryList *_actions = new EntryList();

        //Static methods
        static void addTemplateAction(QString templateType, QString key, QString value, Properties properties = {});
        static void addButtonTemplateActions(QString templateType);
};

typedef QMap<int, ButtonData *> ButtonsDataMap;

#endif // ACTIONS_H
