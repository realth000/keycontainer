#ifndef FINDKEYUI_H
#define FINDKEYUI_H

#include <QDialog>
#include "commoninclude.h"
#include <QMouseEvent>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class FindKeyUi;
}

class FindKeyUi : public QDialog
{
    Q_OBJECT

public:
    explicit FindKeyUi(QWidget *parent = nullptr);
    ~FindKeyUi();
    void setInputFocus();
signals:
    void closed();
    void changeFindText(QString s) const;
    void findTextPrevious() const;
    void findTextNext() const;

public slots:
    void setTransparency(bool pos);
    void setLogLText(QString s);
    void clearLogL();
    void freezeFindBtn() const;
    void unfreezeFindBtn() const;
    bool isFinBtnFreezed() const;

private slots:
    void on_findPreBtn_clicked();
    void on_findNextBtn_clicked();

    void on_findKeywordLE_textChanged(const QString &arg1);

private:
    Ui::FindKeyUi *ui;
    int F3FindMode = 1;// =1时F3控制向下搜索， =0时F3控制向上搜索

    void initUi();
};

#endif // FINDKEYUI_H
