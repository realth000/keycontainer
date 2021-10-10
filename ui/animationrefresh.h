#ifndef ANIMATIONREFRESH_H
#define ANIMATIONREFRESH_H

#include <QGroupBox>
#include <QCheckBox>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QLinkedList>

//#define CCOLOR1 "255,0,0,0"
//#define CCOLOR2 "255,0,0,0"
//#define CCOLOR3 "255,0,0,0"
#define PAD_INITVALUE 4

class AnimationRefresh : public QGroupBox
{
    Q_OBJECT

public:
    AnimationRefresh(QWidget *parent = nullptr);
    ~AnimationRefresh();
    void start(qreal durationtime = 0);
    void setColor(QColor color1, QColor color2, QColor color3);
    QColor getColor(int pos) const;
    void setSpeed(qreal cooldown);
    qreal getSpeed() const;
    bool getRunningState() const;
    void setPad(int pad);
    int getPad() const;

signals:
    void stopped();

public slots:
    void stop();

protected:
    virtual void paintEvent(QPaintEvent *e) override;

private:
    bool runningState = false;
    int widthi;
    int heighti;
    void initUi();
    QRect r11;
    QRect r12;
    QRect r13;
    QRect r21;
    QRect r23;
    QRect r31;
    QRect r32;
    QRect r33;
    QRect painterPos0;
    QRect painterPos1;
    QRect painterPos2;
    QRect painterPos3;
    qreal cooldown = 75; // 刷新动画一帧的间隔，必须大于0
    // NOTE: 三个color不能是rgba，QPen不带透明度，会导致边框颜色更深，达不到无边框的外观
    QColor cColor1 = QColor(137,144,144);
    QColor cColor2 = QColor(118,123,123);
    QColor cColor3 = QColor(104,109,109);
    QColor cColor4 = QColor(71,73,73);
    QTimer aniController; // 控制动画动，每次计时一个cooldown
    QTimer runController;
    QList<QRect> poses;
    int aniPos = 0; // 记录动画进行到第几帧
    qreal runDuration = 5000; // 控制动画的整个显示时长
    QPainter  *painter;
    int pad = PAD_INITVALUE; // 控制Rect间的间隔，必须大于0
};

#endif // ANIMATIONREFRESH_H
