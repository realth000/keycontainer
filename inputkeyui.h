#ifndef INPUTKEYUI_H
#define INPUTKEYUI_H

#include <QDialog>
#include "commoninclude.h"

namespace Ui {
class InputKeyUi;
}

class InputKeyUi : public QDialog
{
    Q_OBJECT

public:
    explicit InputKeyUi(QWidget *parent = nullptr, quint32 id = 0, KeyMap *keyMap = nullptr);
    ~InputKeyUi();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void inputFinish(bool);

private slots:
    void on_inputKey_saveB_clicked();
    void on_showPwdB_pressed();
    void on_showPwdB_released();

private:
    void initUi();
    bool checkInput();

    quint32 tid;
    KeyMap *m_keyMap;
    Ui::InputKeyUi *ui;
    bool showing = false;
};

#endif // INPUTKEYUI_H
