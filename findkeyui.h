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
//    void findText(QString s) const;
    void findTextPrevious(QString s) const;
    void findTextNext(QString s) const;

public slots:
    void setTransparency(bool pos);
    void setLogNotFound();
    void clearLogL();
    void unfreezeFindBtn() const;

private slots:
    void on_findPreBtn_clicked();

    void on_findNextBtn_clicked();

private:
    Ui::FindKeyUi *ui;
    void initUi();
};

#endif // FINDKEYUI_H
