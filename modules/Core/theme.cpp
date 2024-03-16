#include "theme.h"

#include <QGuiApplication>
#include <QScreen>

Theme *Theme::instance()
{
    static Theme singleton;
    QQmlEngine::setObjectOwnership(&singleton, QQmlEngine::CppOwnership);

    return &singleton;
}

Theme *Theme::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    // Create the object using some custom constructor or factory.
    // The QML engine will assume ownership and delete it, eventually.
    return instance();
}

Theme::Theme(QObject *parent)
    : QObject{parent}
{
    QSize rec = QGuiApplication::primaryScreen()->size();

    m_TABLE_MINIMAL_WIDTH = rec.width() / 20;
    m_TABLE_SERVICE_WIDTH = rec.width() / 40;
    m_BUTTON_SELECT_WIDTH = rec.width() / 40;
    /*TODO do calc pref size*/
    m_TABLE_FONTSIZE      = 22;

    m_BASE_WIDTH  = rec.width() / 40;



    m_ICON_COLOR = "black";

    connect(this, &Theme::BASE_HEIGHTChanged, [=](){
        m_BASE_ICON_SIZE = m_BASE_HEIGHT * 0.5;
    });

    setBaseHeight(rec.height() / 24);


}
