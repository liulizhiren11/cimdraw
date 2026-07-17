#include <QApplication>
#include <QByteArray>
#include <QtTest/QtTest>

#include "test_cim_model.h"

int main(int argc, char** argv)
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));

    QApplication app(argc, argv);

    TestCimModel tc;
    return QTest::qExec(&tc, argc, argv);
}
