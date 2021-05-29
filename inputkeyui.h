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
    explicit InputKeyUi(QWidget *parent = nullptr, quint32 id = 0, KeyMap *keyMap = nullptr, QList<Estring> discIndex = QList<Estring>());
    ~InputKeyUi();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void inputFinish(bool result, int existPos);

private slots:
    void on_inputKey_saveB_clicked();
    void on_showPwdB_pressed();
    void on_showPwdB_released();

private:
    void initUi();
    bool checkInput();
    bool checkExistence();

    quint32 tid;
    KeyMap *m_keyMap;
    QList<Estring> m_existsKeys;
    Ui::InputKeyUi *ui;
    bool showing = false;
    int existPos = -1;
};

#endif // INPUTKEYUI_H
