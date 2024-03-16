#ifndef THEME_H
#define THEME_H

#include <QObject>
#include "qqmlintegration.h"

#include <QColor>
#include <QQmlEngine>
#include <QJSEngine>

#include "macros.h"

class Theme : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    UA_PROP_RO(int, TABLE_MINIMAL_WIDTH)
    UA_PROP_RO(int, TABLE_SERVICE_WIDTH)
    UA_PROP_RO(int, TABLE_FONTSIZE)
    UA_PROP_RO(int, BUTTON_SELECT_WIDTH)

    UA_PROP_RW(int, BASE_WIDTH, setBaseWidth)
    UA_PROP_RW(int, BASE_HEIGHT, setBaseHeight)

    UA_PROP_RW(int, theme, setTheme)

    UA_PROP_RW(QColor, TABLE_COLOR_LINES, setTableColorLines)
    UA_PROP_RW(QColor, TABLE_COLOR_EVEN, setTableColorEven)
    UA_PROP_RW(QColor, TABLE_COLOR_ODD, setTableColorOdd)

    UA_PROP_RW(QColor, ICON_COLOR, setIconColor)


    UA_PROP_RO(int, BASE_ICON_SIZE)

    UA_DEF(int, BASE_MARGINS, 5)

public:
    static Theme* instance();
    static Theme* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

private:
    explicit Theme(QObject *parent = nullptr);
    Theme &operator=(const Theme& rhs);

signals:

};

#endif // THEME_H
