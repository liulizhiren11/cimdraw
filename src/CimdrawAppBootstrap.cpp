#include "CimdrawAppBootstrap.h"

#include <QApplication>
#include "CimdrawFrame.h"

int runCimdrawApplication(QApplication& app, const QString& filename)
{
    CimdrawFrame frame(filename);
    if (filename.trimmed().isEmpty())
        frame.newPowerScene();

    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
