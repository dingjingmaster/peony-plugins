//
// Created by dingjing on 3/5/25.
//

#include "andsec-menu.h"

#include <QUrl>
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <peony-qt/file-info.h>

#include <sys/un.h>
#include <sys/socket.h>

#include "common.h"


typedef struct
{
    const char*      src;
    const char*      dest;
} AndsecLanguage;

static gboolean     andsec_language_is_chinese  (void);
static gboolean     andsec_language_is_english  (void);
static const char*  andsec_translate            (const char* str);
static void         andsec_files_execute_cmd    (const QStringList& files, gboolean enc);

static const AndsecLanguage gsChinese[] = {
    { "Andsec", "安得卫士" },
    { "Manual encryption",      "手动加密"},
    { "Privileged decryption",  "特权解密"},
    { "Decryption success",     "解密成功"},
    { "Decryption failed",      "解密失败"},
    { "Encryption success",     "加密成功"},
    { "Encryption failed",      "加密失败"},
    { "Total file",             "文件总数"},
    {nullptr, nullptr},
};


AndsecMenu::AndsecMenu(QObject * parent)
    : QObject(parent), mEnable(true)
{
}

QString AndsecMenu::testPlugin()
{
    return "";
}

bool AndsecMenu::isEnable()
{
    return mEnable;
}

void AndsecMenu::setEnable(bool enable)
{
    mEnable = enable;
}

const QString AndsecMenu::description()
{
    return "Andsec menu";
}

const QString AndsecMenu::name()
{
    return "Andsec menu";
}

const QIcon AndsecMenu::icon()
{
    return QIcon::fromTheme("system-users-symbolic");
}

Peony::PluginInterface::PluginType AndsecMenu::pluginType()
{
    return PluginInterface::MenuPlugin;
}

QList<QAction*> AndsecMenu::menuActions(Types types, const QString & uri, const QStringList & selectionUris)
{
    QList<QAction *> l;

    if (selectionUris.count() <= 0) {
        return l;
    }

    const auto info = Peony::FileInfo::fromUri(selectionUris.first());

    if (selectionUris.first().startsWith("computer:///") || selectionUris.first().startsWith("trash:///") || info->isVirtual()) {
        return l;
    }

    bool e = get_mouse_manual_encrypt();
    bool d = get_privileged_decrypt();

    if (e || d) {
        const auto menu = new QMenu();
        const auto action = new QAction();
        {
            if (e) {
                const auto a = new QAction();
                a->setText(andsec_translate("Manual encryption"));
                menu->addAction(a);
                connect(a, &QAction::triggered, this, [=] (bool) {
                    QStringList files;
                    for (const auto& u: selectionUris) {
                        files << QUrl(u).path();
                    }
                    andsec_files_execute_cmd(files, true);
                });
            }

            if (d) {
                const auto a = new QAction();
                a->setText(andsec_translate("Privileged decryption"));
                menu->addAction(a);
                connect(a, &QAction::triggered, this, [=] (bool) {
                    QStringList files;
                    for (const auto& u: selectionUris) {
                        files << QUrl(u).path();
                    }
                    andsec_files_execute_cmd(files, false);
                });
            }
        }
        action->setText(andsec_translate("Andsec"));
        action->setMenu(menu);
        l << action;
    }

    return l;
}


static gboolean andsec_language_is_english (void)
{
    const char* ls = getenv("LANG");

    return (!ls || (0 == strncmp(ls, "en_US", 5)));
}

static gboolean andsec_language_is_chinese (void)
{
    const char* ls = getenv("LANG");

    return (ls && (0 == strncmp(ls, "zh_CN", 5)));
}

static const char* andsec_translate (const char* str)
{
    if (andsec_language_is_chinese()) {
        int i = 0;
        for (i = 0; nullptr != gsChinese[i].src; ++i) {
            if (0 == strcmp(str, gsChinese[i].src)) {
                return gsChinese[i].dest;
            }
        }
    }

    return str;
}

static void andsec_files_execute_cmd(const QStringList& files, gboolean enc)
{
    g_return_if_fail(!files.isEmpty());

    const QString allFiles = files.join("|");

    printf("%s\n", allFiles.toUtf8().data());

    // 发送文件
    send_data_to_daemon((enc ? IPC_TYPE_PRIVILEGED_ENCRYPT_FILES_SYNC : IPC_TYPE_PRIVILEGED_DECRYPT_FILES_SYNC), andsec_language_is_chinese(), allFiles.toUtf8().data(), allFiles.toUtf8().size());
}

