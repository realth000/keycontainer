#ifndef MESSAGEBOXEX_H
#define MESSAGEBOXEX_H

#include <QDialog>
#include <QMessageBox>


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class MessageBoxEx;
}

class MessageBoxEx : public QDialog
{
    Q_OBJECT
    // Keep these enums sync with them in qmessagebox.h
    // current sync with Qt 5.12.10
public:
    enum Icon{
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };
    Q_ENUM(Icon)

    enum ButtonRole {
        // keep this in sync with QDialogButtonBox::ButtonRole and QPlatformDialogHelper::ButtonRole
        InvalidRole = -1,
        AcceptRole,
        RejectRole,
        DestructiveRole,
        ActionRole,
        HelpRole,
        YesRole,
        NoRole,
        ResetRole,
        ApplyRole,

        NRoles
    };
    Q_ENUM(ButtonRole)

    enum StandardButton {
        // keep this in sync with QDialogButtonBox::StandardButton and QPlatformDialogHelper::StandardButton
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,

        FirstButton        = Ok,                // internal
        LastButton         = RestoreDefaults,   // internal

        YesAll             = YesToAll,          // obsolete
        NoAll              = NoToAll,           // obsolete

        Default            = 0x00000100,        // obsolete
        Escape             = 0x00000200,        // obsolete
        FlagMask           = 0x00000300,        // obsolete
        ButtonMask         = ~FlagMask          // obsolete
    };
    Q_ENUM(StandardButton)

    explicit MessageBoxEx(QWidget *parent = nullptr);
    ~MessageBoxEx();
    void information(QString titleText = "提示", QString text = "", QString buttonText = "确定");
    int warning(QString titleText = "警告", QString text = "", QString yesText = "确定", QString noText = "取消");

private:
    Ui::MessageBoxEx *ui;
    int result = 0x00000000;

private slots:
    void resultToYes();
    void resultToNo();

};

#endif // MESSAGEBOXEX_H
