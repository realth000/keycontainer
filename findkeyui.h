#ifndef FINDKEYUI_H
#define FINDKEYUI_H

#include <QDialog>
#include "commoninclude.h"
#include <QKeyEvent>

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
    bool isFindAllWord() const;
    Qt::CaseSensitivity isCaseSen() const;

signals:
    void closed();
    void changeFindText(QString s);
    void findTextPrevious();
    void findTextNext();
    void countAll();
    void findKeyUseRegSig(int use);

public slots:
    void setTransparency(bool pos);
    void setLogLText(QString s);
    void clearLogL();
    void freezeFindBtn() const;
    void unfreezeFindBtn() const;
    bool isFinBtnFreezed() const;

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;

private slots:
    void on_findPreBtn_clicked();
    void on_findNextBtn_clicked();
    void on_findKeywordLE_textChanged(const QString &arg1);
    void on_countBtn_clicked();
    void on_useRegChB_stateChanged(int arg1);
    void on_findAllWordChB_stateChanged(int arg1);
    void on_findCaseSensitivityChB_stateChanged(int arg1);

private:
    Ui::FindKeyUi *ui;
    int F3FindMode = 1; // =1时F3控制向下搜索， =0时F3控制向上搜索
    bool findDirection = true; // =true，快捷键F3控制向下搜索，=false，快捷键F3控制向上搜索
    bool findAllWord = false; // 全词查找
    Qt::CaseSensitivity findCaseSen = Qt::CaseInsensitive; // 匹配大小写，默认不敏感(false, =0)

    void initUi();
};

#endif // FINDKEYUI_H
