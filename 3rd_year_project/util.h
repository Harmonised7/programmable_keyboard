#ifndef UTIL_H
#define UTIL_H

#include "action.h"
#include "prefs.h"

#include <QMap>
#include <QDebug>
#include <QList>
#include <QString>
#include <QTreeWidgetItem>

class Util
{
public:
    static void clearQTreeWidgetItemChildren( QTreeWidgetItem *item )
    {
        for( QTreeWidgetItem *child : item->takeChildren() )
        {
            item->removeChild( child );
        }
    }

    private:
};

#endif // UTIL_H
