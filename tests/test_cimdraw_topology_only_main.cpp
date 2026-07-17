#include <QApplication>
#include <QByteArray>
#include <QtTest/QtTest>

#include "test_cimdraw_topology.h"

int main(int argc, char** argv)
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));

    QApplication app(argc, argv);

    TestCimdrawTopology tc;
    return QTest::qExec(&tc, argc, argv);
}
