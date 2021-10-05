#include "qlineeditpro.h"
#include <QEvent>
QLineEditPro::QLineEditPro(QWidget *parent):
    QLineEdit (parent)
{

}

QLineEditPro::~QLineEditPro()
{

}

void QLineEditPro::mouseDoubleClickEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton){
        mouseEvent->accept();
        emit lineEditDblClicked(this);
    }
}
