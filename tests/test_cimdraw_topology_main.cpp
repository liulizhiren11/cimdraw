#include <QApplication>
#include <QByteArray>
#include <QtTest/QtTest>

#include "test_cimdraw_topology.h"
#include "test_cim_model.h"
#include "test_cim_importer.h"
#include "test_cim_ui.h"

int main(int argc, char** argv)
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));

    QApplication app(argc, argv);

    int status = 0;

    {
        TestCimdrawTopology tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestCimModel tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestCimImporter tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        TestCimUi tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    return status;
}
