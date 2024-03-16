#ifndef DATAPIPE_H
#define DATAPIPE_H

#include <QObject>
#include <QSettings>

#include <QQuickItem>
#include <QMutex>

#include "sqldatabase.h"
#include "defs.h"

class Datapipe : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    static Datapipe* instance();
    static Datapipe* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    ~Datapipe();

    Q_INVOKABLE QSettings* getSettings();
    Q_INVOKABLE QVariant settings(const QString &name, const QVariant &deflt = QVariant());

    Q_INVOKABLE QVariant variable(const QString &name, QVariant deflt = QVariant());
    Q_INVOKABLE void setVariable(const QString &name, QVariant var);

    Q_INVOKABLE int nextIid();

public:
    Q_INVOKABLE SqlDatabase* sql(const QString conn = Defs::DEFAULT_CONNECTION);


private:
    explicit Datapipe();
    Datapipe &operator=(const Datapipe& rhs);

    mutable QMutex sqlMutex;
    QMap<QString, SqlDatabase*> m_sqlMap;

    QSettings* m_settings;
    QVariantMap m_variables;

    mutable QMutex iidMutex;
    int iid;
};

#endif // DATAPIPE_H
