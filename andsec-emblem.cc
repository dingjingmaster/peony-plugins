//
// Created by dingjing on 3/5/25.
//

#include "andsec-emblem.h"

#include <QUrl>

extern int MonIsEncryptFile (const char* pFileName);

QStringList EmblemProvider::getFileEmblemIcons(const QString& uri)
{
    printf("%s\n", uri.toStdString().c_str());
    if (MonIsEncryptFile(QUrl(uri).path().toUtf8().data())) {
        return QStringList() << "andsec_lock";
    }

    return {};
}

AndsecEmblem::AndsecEmblem(QObject * parent)
    : QObject(parent), mEnable(true)
{
}

bool AndsecEmblem::isEnable()
{
    return mEnable;
}

void AndsecEmblem::setEnable(bool enable)
{
    mEnable = enable;
}

const QString AndsecEmblem::description()
{
    return "";
}

const QString AndsecEmblem::name()
{
    return "Andsec Emblem";
}

const QIcon AndsecEmblem::icon()
{
    return QIcon();
}

Peony::PluginInterface::PluginType AndsecEmblem::pluginType()
{
    return Peony::PluginInterface::EmblemPlugin;
}

Peony::EmblemProvider * AndsecEmblem::create()
{
    return new EmblemProvider();
}
