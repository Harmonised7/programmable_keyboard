#ifndef ACTIONS_H
#define ACTIONS_H

#include "util.h"
#include "entry.h"
#include "prefs.h"

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
        static QMap<int, ButtonData*> buttonsDataFromJson(QJsonObject jsonArray);
        static void setButtonData(int buttonIndex, ButtonData *data);
        static void setButtonsData(QMap<int, ButtonData*> buttonsData);
        static void wipeButtonsData();
        static QString getToAlias(QString word);
        static QString getFromAlias(QString word);
        static int getToAsciiAlias(QString word);
        static QString getFromAsciiAlias(int ascii);

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
        inline static const QString PRESS = "press";
//        inline static const QString HOLD = "hold";
//        inline static const QString RELEASE = "release";
        inline static const QList<QString> DATA_KEYS = {/*MISC,*/ PRESS/*, HOLD, RELEASE*/};

    private:
        //Static fields
        inline static TemplateEntriesMap *_templateEntriesMap = new TemplateEntriesMap;
        inline static QMap<int, ButtonData*> *_buttonsDataMap = new QMap<int, ButtonData*>;
        inline static QMap<QString, QString> *_aliasToMap = new QMap<QString, QString>;
        inline static QMap<QString, QString> *_aliasFromMap = new QMap<QString, QString>;

        inline static QMap<int, QString> *_asciiToWordMap = new QMap<int, QString>;
        inline static QMap<QString, int> *_wordToAsciiMap = new QMap<QString, int>;

        //Fields
        EntryListMap *_data = new EntryListMap();
//        EntryList *_actions = new EntryList();

        //Static methods
        static void addTemplateAction(QString templateType, QString key, QString value, Properties properties = {});
        static void addButtonTemplateActions(QString templateType);
        static void registerAlias(QString a, QString b);
        static void registerAsciiAlias(int a, QString b);
        static void registerAliases();
};

typedef QMap<int, ButtonData *> ButtonsDataMap;

#endif // ACTIONS_H
