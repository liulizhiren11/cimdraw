#ifndef CIMBEHAVIORRUNTIMEINGEST_H
#define CIMBEHAVIORRUNTIMEINGEST_H

#include <QString>
#include <QVariant>

class CimdrawScene;

enum class CimBehaviorRuntimeIngestKind
{
    Rejected = 0,
    RawRuntimeValue,
    BehaviorResultPayload,
};

struct CimBehaviorRuntimeIngestResult
{
    CimBehaviorRuntimeIngestKind kind = CimBehaviorRuntimeIngestKind::Rejected;

    bool accepted() const
    {
        return kind != CimBehaviorRuntimeIngestKind::Rejected;
    }

    bool behaviorResultPayload() const
    {
        return kind == CimBehaviorRuntimeIngestKind::BehaviorResultPayload;
    }
};

CimBehaviorRuntimeIngestResult cimApplyWiringRuntimeValue(CimdrawScene* scene,
                                                          const QString& key,
                                                          const QVariant& value);

#endif
