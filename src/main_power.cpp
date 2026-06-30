#include <QApplication>
#include "LzAppBootstrap.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName(QString::fromUtf8("lz-power"));
    app.setApplicationDisplayName(QString::fromUtf8("电力工程"));
    QString filename;
    for (int i = 1; i < argc; ++i)
    {
        const QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith(QLatin1String("--")))
            continue;
        filename = arg;
        break;
    }

    return lzRunApplication(app, filename);
}
