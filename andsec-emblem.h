//
// Created by dingjing on 3/5/25.
//

#ifndef peony_plugins_ANDSEC_EMBLEM_H
#define peony_plugins_ANDSEC_EMBLEM_H
#include <QObject>
#include <peony-qt/emblem-plugin-iface.h>


class EmblemProvider final : public Peony::EmblemProvider
{
    Q_OBJECT
public:
    QStringList getFileEmblemIcons(const QString &uri) override;
};

class AndsecEmblem final : public QObject, public Peony::EmblemPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EmblemPluginInterface_iid FILE "common.json")
    Q_INTERFACES(Peony::EmblemPluginInterface)
public:
    explicit AndsecEmblem(QObject *parent = nullptr);

    bool isEnable() override;
    void setEnable(bool enable) override;
    const QString description() override;
    const QString name() override;
    const QIcon icon() override;
    PluginInterface::PluginType pluginType() override;
    virtual Peony::EmblemProvider *create() override;

private:
    bool                                mEnable;
};



#endif // peony_plugins_ANDSEC_EMBLEM_H
