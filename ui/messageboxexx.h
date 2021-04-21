#ifndef MessageBoxExX_H
#define MessageBoxExX_H

#include <QDialog>
#include <QMessageBox>


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class MessageBoxExX;
}

class MessageBoxExX : public QDialog
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
        NoButton           = QMessageBox::NoButton,
        Ok                 = QMessageBox::Ok,
        Save               = QMessageBox::Save,
        SaveAll            = QMessageBox::SaveAll,
        Open               = QMessageBox::Open,
        Yes                = QMessageBox::Yes,
        YesToAll           = QMessageBox::YesToAll,
        No                 = QMessageBox::No,
        NoToAll            = QMessageBox::NoToAll,
        Abort              = QMessageBox::Abort,
        Retry              = QMessageBox::Retry,
        Ignore             = QMessageBox::Ignore,
        Close              = QMessageBox::Close,
        Cancel             = QMessageBox::Cancel,
        Discard            = QMessageBox::Discard,
        Help               = QMessageBox::Help,
        Apply              = QMessageBox::Apply,
        Reset              = QMessageBox::Reset,
        RestoreDefaults    = QMessageBox::RestoreDefaults,

        FirstButton        = QMessageBox::FirstButton,                // internal
        LastButton         = QMessageBox::LastButton,   // internal

        YesAll             = QMessageBox::YesAll,          // obsolete
        NoAll              = QMessageBox::NoAll,           // obsolete

        Default            = QMessageBox::Default,        // obsolete
        Escape             = QMessageBox::Escape,        // obsolete
        FlagMask           = QMessageBox::FlagMask,        // obsolete
        ButtonMask         = QMessageBox::ButtonMask          // obsolete
    };
    Q_ENUM(StandardButton)

    explicit MessageBoxExX(QWidget *parent = nullptr);
    ~MessageBoxExX();
    void information(QString titleText = "提示", QString text = "", QString buttonText = "确定");
    int warning(QString titleText = "警告", QString text = "", QString yesText = "确定", QString noText = "取消");
    int question(QString titleText = "?", QString text = "", QString yesText = "是", QString noText = "否");

private:
    Ui::MessageBoxExX *ui;
    int result = 0x00000000;

private slots:
    void resultToYes();
    void resultToNo();

};

#endif // MessageBoxExX_H
