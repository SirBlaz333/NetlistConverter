#include "src/app/app_settings.h"
#include "src/ui/mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>

#include <src/console/console_app.h>

QString initStyles();

int main(int argc, char *argv[])
{
    AppSettings::loadSettings();
    if (argc > 1) {
        ConsoleApplication console(argc, argv);
        return console.exec();
    }
    QApplication app(argc, argv);
    MainWindow window;
    app.setStyleSheet(initStyles());
    window.show();
    return app.exec();
}

QString initStyles()
{
    QFile stylesheet(":styles/stylesheet.qss");
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styles = stylesheet.readAll();
        stylesheet.close();
        return styles;
    }
    return "";
}
