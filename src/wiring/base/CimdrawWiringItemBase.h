#ifndef CIMDRAWWIRINGITEMBASE_H
#define CIMDRAWWIRINGITEMBASE_H

#include "Item/CimdrawItem.h"
#include "CimdrawWiringTypes.h"
#include "CimdrawWiringVisualState.h"
#include "cim/behavior/CimBehaviorResult.h"
#include "cim/query/CimGraphicRenderState.h"
#include "topology/PowerTopologyTypes.h"

#include <QGraphicsSceneContextMenuEvent>
#include <QVariant>

class QGraphicsSceneMouseEvent;
class QPainterPath;
class QTimer;
class CimdrawScene;
class CimdrawConnectPoint;

class CimdrawWiringItemBase : public CimdrawItem
{
    Q_OBJECT
    Q_PROPERTY(QString referenceDesignator READ referenceDesignator WRITE setReferenceDesignator NOTIFY referenceDesignatorChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString valueText READ valueText WRITE setValueText NOTIFY valueTextChanged)
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelNameChanged)
    Q_PROPERTY(QString wiringDataKey READ wiringDataKey WRITE setWiringDataKey NOTIFY wiringDataKeyChanged)
    Q_PROPERTY(int wiringRunState READ wiringRunStateInt WRITE setWiringRunStateInt NOTIFY wiringRunStateChanged)
    /** 断路器/刀闸触点：0=分闸(开)，1=合闸 */
    Q_PROPERTY(int switchPosition READ switchPosition WRITE setSwitchPosition NOTIFY switchPositionChanged)
    Q_PROPERTY(int flowSign READ flowSign WRITE setFlowSign NOTIFY flowSignChanged)

signals:
    void referenceDesignatorChanged(const QString& text);
    void displayNameChanged(const QString& text);
    void valueTextChanged(const QString& text);
    void modelNameChanged(const QString& text);
    void wiringDataKeyChanged(const QString& key);
    void wiringRunStateChanged(int state);
    void switchPositionChanged(int pos);
    void flowSignChanged(int sign);

protected:
    explicit CimdrawWiringItemBase(CimdrawWiringItemKind kind, QGraphicsItem* parent = nullptr);
    CimdrawWiringItemBase(CimdrawWiringItemKind kind, const QRectF& pos, QGraphicsItem* parent = nullptr);
    ~CimdrawWiringItemBase() override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) override;
    void installConnectPointsFromRect(const QRectF& pos);
    void finalizeWiringConstruction();

public:
    CimdrawWiringItemKind sldKind() const { return kind_; }

    QString referenceDesignator() const { return referenceDesignator_; }
    void setReferenceDesignator(const QString& text);

    QString displayName() const override;
    void setDisplayName(const QString& text);

    QString valueText() const { return valueText_; }
    void setValueText(const QString& text);

    QString modelName() const { return modelName_; }
    void setModelName(const QString& text);

    QString wiringDataKey() const { return wiringDataKey_; }
    void setWiringDataKey(const QString& k);

    CimdrawWiringRunState wiringRunState() const { return runState_; }
    void setWiringRunState(CimdrawWiringRunState s);
    void applyBehaviorResult(const CimBehaviorResult& result);
    CimBehaviorResult currentBehaviorResult() const { return behaviorResult_; }
    CimBehaviorResult effectiveBehaviorResult() const;
    CimdrawWiringRunState effectiveWiringRunState() const;
    bool effectiveAlarmPulse() const;
    int effectiveFlowSign() const;
    CimGraphicRenderStateSource graphicRenderStateSource() const;
    CimdrawWiringVisualState graphicRenderState() const;

    int wiringRunStateInt() const;
    void setWiringRunStateInt(int v);

    int flowSign() const { return flowSign_; }
    void setFlowSign(int sign);

    /** 0=分闸(开)，1=合闸；与 Q_PROPERTY switchPosition 一致 */
    int switchPosition() const;
    void setSwitchPosition(int v);
    int effectiveSwitchPosition() const;

    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateCoordinate() override;
    bool saveXml(QDomElement* g) override;
    bool loadXml(QDomElement* g) override;
    void stretch(int handle, double sx, double sy, const QPointF& origin) override;
    bool isTopologyGraphNode() const override;
    bool participatesInTopology() const override;
    CimdrawTopologyDomain topologyDomain() const override;
    virtual PowerTopologyRole powerTopologyRole() const;
    virtual QHash<QString, bool> powerTopologyInternalConnectivity(const QVector<QString>& terminalIds,
                                                                   int effectiveSwitchPosition) const;
    void setState(HANDLE_STATE st) override;

    virtual CimdrawDrawTypeId drawTypeForXml() const = 0;
    virtual CimdrawWiringItemBase* cloneForDuplicate() const = 0;
    QGraphicsItem* duplicate() override;

    /** 母线等：沿导体边动态生成连接点，无固定左右端口 */
    virtual bool wiringUsesAlongEdgeConnectPoints() const { return false; }
    /** 由具体图元实例决定端口数量与布局的自定义连接点 */
    virtual bool wiringUsesCustomConnectPoints() const { return false; }
    virtual int customWiringConnectPointCount() const { return -1; }
    virtual void refreshWiringCustomConnectPoints();
    virtual CimdrawConnectPoint* ensureConnectPointAtScene(const QPointF& scenePos);
    virtual void rebindAlongEdgeConnectPorts();
    virtual bool flowSignAffectsConnectPoints() const { return false; }
    virtual bool supportsFlowDirectionAction() const { return false; }
    virtual bool supportsSwitchToggleOnDoubleClick() const { return false; }
    virtual bool topologyGraphNodeEnabled() const { return true; }
    virtual bool wiringUsesVisualTimer() const { return false; }

    /** 当前全局接线图符号标准（美标 / IEC / 国标）；绘制与后续符号分岔均以此为入口 */
    CimdrawWiringSymbolStandard wiringSymbolStandard() const;

protected:
    virtual int defaultWiringConnectPointCount() const;
    virtual void configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const;
    virtual QPainterPath defaultRoutingObstaclePath() const;
    void copyWiringMetadataFrom(const CimdrawWiringItemBase* o);
    void copyWiringRuntimeFrom(const CimdrawWiringItemBase* o);
    virtual void copyCustomDataFrom(const CimdrawWiringItemBase* o);
    virtual void saveCustomXml(QDomElement* g) const;
    virtual void loadCustomXml(QDomElement* g);
    bool breakerVisualOpen() const;
    bool publishCurrentBehaviorResultToScene();
    bool toggleSwitchPositionFromInteraction();
    CimdrawWiringVisualState graphicRenderStateForPaint() const;
    CimdrawWiringVisualState prepareGraphicRenderStateForPaint(QPainter* painter);
    void connectSceneWiringSignals(CimdrawScene* sc);
    void disconnectSceneWiringSignals(CimdrawScene* sc);

    const CimdrawWiringItemKind kind_;
    QString referenceDesignator_;
    QString displayName_;
    QString valueText_;
    QString modelName_;
    QString wiringDataKey_;
    CimdrawWiringRunState runState_ = CimdrawWiringRunState::Normal;
    int flowSign_ = 1;
    bool switchContactsClosed_ = true;
    CimBehaviorResult behaviorResult_;
    bool applyingBehaviorResult_ = false;

private slots:
    void onSceneWiringDataChanged(const QString& key, const QVariant& value);
    void onWiringVisualTick();

private:
    void applyWiringPayload(const QVariant& value);
    bool resolveGraphicAlarmPulse(CimdrawWiringRunState runState) const;
    void updateWiringVisualTimer();
    void rebindWiringConnectPorts();
    void reconcileWiringConnectPoints();

    QTimer* wiringVisualTimer_ = nullptr;
};

#endif
