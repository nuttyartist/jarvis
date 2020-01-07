#include "mainwindow.h"
#include "texttospeech.h"

#include <QApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QtCore/QFile>
#include <QtWidgets/QMainWindow>
#include <QtQml/QQmlContext>
#include <QDir>
#include <QDebug>

#include <exception>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow main_window;

    if(!QDir(QStringLiteral("%1/.deus").arg(QDir::homePath())).exists())
    {
        // Setup
        QDir().mkdir(QStringLiteral("%1/.deus").arg(QDir::homePath()));
    }

    main_window.show();

    return app.exec();
}
