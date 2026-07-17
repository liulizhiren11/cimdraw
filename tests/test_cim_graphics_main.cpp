#include <QApplication>
#include <QByteArray>
#include <QtTest/QtTest>

#include "test_cim_graphics.h"

int main(int argc, char** argv)
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));

    QApplication app(argc, argv);

    TestCimGraphics tc;
    return QTest::qExec(&tc, argc, argv);
}
