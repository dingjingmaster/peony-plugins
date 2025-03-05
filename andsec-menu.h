//
// Created by dingjing on 3/5/25.
//

#ifndef peony_plugins_ANDSEC_MENU_H
#define peony_plugins_ANDSEC_MENU_H
#include <QObject>
#include <QAction>
#include <peony-qt/menu-plugin-iface.h>

class AndsecMenu final : public QObject, public Peony::MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid FILE "common.json")
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit AndsecMenu(QObject *parent = nullptr);

    QString testPlugin() override;
    bool isEnable() override;
    void setEnable(bool enable) override;
    const QString description() override;
    const QString name() override;
    const QIcon icon() override;
    PluginInterface::PluginType pluginType() override;
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

private:
    bool                                mEnable;
};



#endif // peony_plugins_ANDSEC_MENU_H
