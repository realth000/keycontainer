#ifndef TABLEWIDGETEX_H
#define TABLEWIDGETEX_H

#include <QTableWidget>
#include <QObject>

class TableWidgetEx : public QTableWidget
{
    Q_OBJECT
public:
    TableWidgetEx(QWidget *parent);
    int currentRowNumber = -1;
    int rowCountNumber = 0;

signals:
    void rowInserted(int row);
    void rowRemoved(int row);

public slots:
    void switchToRow(int row);
    void insertRow(int row);
    void removeRow(int row);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;


};

#endif // TABLEWIDGETEX_H
