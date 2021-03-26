#ifndef QLINEEDITPRO_H
#define QLINEEDITPRO_H

#include <QLineEdit>
#include <QMouseEvent>

class QLineEditPro : public QLineEdit
{
    Q_OBJECT
public:
    QLineEditPro(QWidget *parent = nullptr);
    ~QLineEditPro();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *mouseEvent) override;

signals:
    void lineEditDblClicked(QLineEditPro *);

};

#endif // QLINEEDITPRO_H
