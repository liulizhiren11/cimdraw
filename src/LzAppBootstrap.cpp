#include "LzAppBootstrap.h"

#include <QApplication>
#include "LzFrame.h"

int lzRunApplication(QApplication& app, const QString& filename)
{
    LzFrame frame(filename);
    if (filename.trimmed().isEmpty())
        frame.newPowerScene();

    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
