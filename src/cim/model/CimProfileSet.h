#ifndef CIMPROFILESET_H
#define CIMPROFILESET_H

#include <QString>
#include <QStringList>

class CimProfileSet
{
public:
    void addProfile(const QString& profileName);
    bool contains(const QString& profileName) const;
    QStringList allProfiles() const;

private:
    QStringList profiles_;
};

#endif // CIMPROFILESET_H
