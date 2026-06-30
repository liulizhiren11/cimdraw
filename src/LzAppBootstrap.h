#ifndef LZAPPBOOTSTRAP_H
#define LZAPPBOOTSTRAP_H

#include <QString>

class QApplication;

int lzRunApplication(QApplication& app, const QString& filename = QString());

#endif
