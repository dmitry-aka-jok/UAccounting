#ifndef DEFS_H
#define DEFS_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#include "macros.h"

class Defs : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE("Class only exposes constants to QML.")
//    QML_ELEMENT
    QML_SINGLETON

public:
    UA_DEF(QString, DEFAULT_CONNECTION,  u"default connection"_qs)

    // special symbols
    UA_DEF(QString, DOT, u"."_qs)
    UA_DEF(QString, COMMA, u","_qs)
    UA_DEF(QString, ASTERISK, u"*"_qs)
    UA_DEF(QString, APOS, u"'"_qs)
    UA_DEF(QString, SPACE, u" "_qs)
    UA_DEF(QString, EMPTY_STRING, u""_qs)
    UA_DEF(QString, BRACKET_OPEN, u"("_qs)
    UA_DEF(QString, BRACKET_CLOSE, u")"_qs)
    UA_DEF(QString, EQ, u"="_qs)
    UA_DEF(QString, NE, u"!="_qs)
    UA_DEF(QString, GT, u">"_qs)
    UA_DEF(QString, GTE, u"=>"_qs)
    UA_DEF(QString, LT, u"<"_qs)
    UA_DEF(QString, LTE, u"<="_qs)

    UA_DEF(QString, SOLUTION, u"solution"_qs)
    // debug level 0 - without, 1 - errors only, 2 - all queries
    UA_DEF(QString, DEBUG_LEVEL, u"debug_level"_qs)

    UA_DEF(QString, SETTINGS_TABLE, u"uac_settings"_qs)
    UA_DEF(QString, SETTINGS_VALUE, u"value"_qs)
    UA_DEF(QString, SETTINGS_SECTION, u"section"_qs)
    UA_DEF(QString, SETTINGS_OPTION, u"option"_qs)
    UA_DEF(QString, SETTINGS_USER, u"user"_qs)
    /*
    static Defs* instance()
    {
        static Defs singleton;
        QQmlEngine::setObjectOwnership(&singleton, QQmlEngine::CppOwnership);

        return &singleton;
    }
    static Defs* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        // Create the object using some custom constructor or factory.
        // The QML engine will assume ownership and delete it, eventually.
        return instance();
    }
*/
private:
    /*
    explicit Defs(){};
    Defs &operator=(const Defs& rhs);
    */

//    explicit Defs(){};
};

#endif // DEFS_H
