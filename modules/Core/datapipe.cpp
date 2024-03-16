#include "datapipe.h"

#include <QCoreApplication>
//#include <QNetworkInterface>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QQmlEngine>

#include <QJSEngine>
#include <QGuiApplication>



Datapipe::Datapipe() : QObject(nullptr), iid(0)
{
    QString s = QCoreApplication::applicationDirPath() + "/" + QCoreApplication::applicationName();

    m_settings = new QSettings(s + ".ini", QSettings::IniFormat);

}


Datapipe *Datapipe::instance()
{
    static Datapipe singleton;
    QQmlEngine::setObjectOwnership(&singleton, QQmlEngine::CppOwnership);

    return &singleton;
}

Datapipe *Datapipe::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    // Create the object using some custom constructor or factory.
    // The QML engine will assume ownership and delete it, eventually.
    return instance();
}

Datapipe::~Datapipe()
{
    delete m_settings;
}

QSettings* Datapipe::getSettings()
{
    return m_settings;
}

QVariant Datapipe::settings(const QString &name, const QVariant &deflt)
{
    return m_settings->value(name, deflt);
}

QVariant Datapipe::variable(const QString &name, QVariant deflt)
{
    if(m_variables.value(name).isValid())
        return m_variables.value(name);
    else
        return deflt;
}

void Datapipe::setVariable(const QString &name, QVariant var)
{
    m_variables[name] = var;
}


int Datapipe::nextIid()
{
    QMutexLocker locker(&iidMutex);

    return ++iid;
}



SqlDatabase *Datapipe::sql(const QString conn)
{
    QMutexLocker locker(&sqlMutex);

    QString currConn = conn;

    if(currConn.isEmpty())
        currConn = Defs::DEFAULT_CONNECTION;

//    qDebug()<<"Datapipe::sql"<<currConn<<m_sqlMap.contains(currConn)<<m_sqlMap.count();

    if(!m_sqlMap.contains(currConn)){
        SqlDatabase *sql = new SqlDatabase(currConn);
        QQmlEngine::setObjectOwnership(sql, QQmlEngine::CppOwnership);
        m_sqlMap[currConn] = sql;
    }
    return m_sqlMap[currConn];
}









