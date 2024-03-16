#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>

#include "build_defs.h"

#include "modules/Core/datapipe.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName("Majister");
    app.setOrganizationDomain("uvelirsoft.com.ua");
    app.setApplicationName("UAccounting");

    QString version = QString::number(BUILD_YEAR)+""
                      +QString::number(BUILD_MONTH).rightJustified(2, '0')+""
                      +QString::number(BUILD_DAY).rightJustified(2, '0')+"."
                      +QString::number(BUILD_HOUR).rightJustified(2, '0')+""
                      +QString::number(BUILD_MIN).rightJustified(2, '0');

    app.setApplicationVersion(version);
#ifdef QT_DEBUG
    //qSetMessagePattern("[%{type}]%{file}:%{line} (%{function}) > %{message}");
    qSetMessagePattern(QLatin1String(
        "%{if-info}\x1b[37mInfo in %{endif}"
        "%{if-debug}\x1b[32mDebug in %{endif}"
        "%{if-warning}\x1b[36mWarning in %{endif}"
        "%{if-critical}\x1b[38mCritical error in %{backtrace depth=5 separator=\"\n---\"}\n%{endif}"
        "%{if-fatal}\x1b[38mFatal error in %{backtrace depth=5 separator=\"\n---\"}\n%{endif}"
        "%{file}:%{line} (%{function}) "
        "\x1b[37m"
        "%{if-category}%{category} %{endif}%{message}"
        "\x1b[0m"
        ));
#else
    qSetMessagePattern("[%{type}] %{message}");
#endif
    //    qInfo()<<1;
    //    qDebug()<<2;
    //    qWarning()<<3;
    //    qCritical()<<4;
    //    qFatal("UPS");



    QCommandLineParser parser;
    parser.setApplicationDescription("Software for accounting and more");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("database", "Database connection string");

    QCommandLineOption optionDriver({"d","driver"},
                                    "Driver for database. Currently supported QPLSQL and QSQLITE.",
                                    "driver");
    parser.addOption(optionDriver);

    QCommandLineOption optionUser({"u","user"},
                                  "User for connect to database.",
                                  "user");
    parser.addOption(optionUser);

    QCommandLineOption optionPassword({"p","password"},
                                      "Password to connect to database.",
                                      "password");
    parser.addOption(optionPassword);

    QCommandLineOption optionOptions({"o","options"},
                                     "Options to connect to database.",
                                     "options");
    parser.addOption(optionOptions);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if(args.length()!=0){
        QString database = args.at(0);

        SqlDatabase *db = Datapipe::instance()->sql();

        db->setDatabase(database);
        if(parser.isSet(optionDriver))
            db->setDriver(parser.value(optionDriver));
        else
            if(database.endsWith(".sqlite")){
                db->setDriver("QSQLITE");
            }

        if(parser.isSet(optionUser))
            db->setUser(parser.value(optionUser));
        if(parser.isSet(optionPassword))
            db->setPassword(parser.value(optionPassword));
        if(parser.isSet(optionOptions))
            db->setOptions(parser.value(optionOptions));

        db->init();
    }


    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("UAccounting", "Main");

    return app.exec();
}
