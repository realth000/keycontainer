#ifndef INPUTINITKEYUI_H
#define INPUTINITKEYUI_H

#include <QDialog>
#include "encryption/encrypted.h"

namespace Ui {
class InputInitKeyUi;
}

class InputInitKeyUi : public QDialog
{
    Q_OBJECT

public:
    explicit InputInitKeyUi(QWidget *parent = nullptr, Estring oldPwdHash = Estring());
    ~InputInitKeyUi();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_inputKey_saveB_clicked();
    void on_showPwdB_pressed();
    void on_showPwdB_released();

private:
    void initUi();
    void checkInput();
    bool getNewPwdHash();
    const QString salt1 = "15^vAd[74AC'v7456.sdO&Pv61v铸下这铁链，江东天险牢不可破";
    const QString salt2 = "離れない君といた夏のおわりゼロゼロさてんるいこおかえり";
    const Estring m_oldPwdHash;
    Ui::InputInitKeyUi *ui;
    bool showing = false;
};

#endif // INPUTINITKEYUI_H
