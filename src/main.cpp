#include <QApplication>
#include "CimdrawAppBootstrap.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName(QString::fromUtf8("cimdraw"));
    app.setApplicationDisplayName(QString::fromUtf8("CIMDraw"));
    QString filename;
    for (int i = 1; i < argc; ++i)
    {
        const QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith(QLatin1String("--")))
            continue;
        filename = arg;
        break;
    }

    return runCimdrawApplication(app, filename);
}
