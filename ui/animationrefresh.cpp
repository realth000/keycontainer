#include "animationrefresh.h"
#include <QDebug>

AnimationRefresh::AnimationRefresh(QWidget *parent):
QGroupBox(parent)
{
    this->setVisible(false);
    aniController.setSingleShot(false);
    painter = new QPainter();
    connect(&runController, &QTimer::timeout, this, &AnimationRefresh::stop, Qt::UniqueConnection);
    connect(&aniController, &QTimer::timeout, this, [this](){
        if(aniPos>=8){aniPos=0;}
        painterPos0 = poses[aniPos];
        painterPos3 = poses[(aniPos+1)%8];
        painterPos2 = poses[(aniPos+2)%8];
        painterPos1 = poses[(aniPos+3)%8];
        aniPos++;
        update();
    }, Qt::UniqueConnection);
}

AnimationRefresh::~AnimationRefresh()
{
    delete painter;
}

void AnimationRefresh::start(qreal durationtime)
{
    initUi();
    painterPos0 = r11;
    painterPos1 = r12;
    painterPos2 = r13;
    painterPos3 = r23;
    this->setVisible(true);
    // TODO: 实际上这里不是特别同步
    if(durationtime > 0){
        runDuration = durationtime;
    }
    runController.start(runDuration);
    aniController.start(cooldown);
    runningState = true;
}

void AnimationRefresh::setColor(QColor color1, QColor color2, QColor color3)
{
    cColor1 = color1;
    cColor2 = color2;
    cColor3 = color3;
}

QColor AnimationRefresh::getColor(int pos) const
{
    switch (pos) {
    case 1:
        return cColor1;
    case 2:
        return cColor2;
    case 3:
        return cColor3;
    default:
        return QColor();
    }
}

void AnimationRefresh::setSpeed(qreal cooldown)
{
    if(cooldown <= 0){
        return;
    }
    this->cooldown = cooldown;
}

qreal AnimationRefresh::getSpeed() const
{
    return this->cooldown;
}

bool AnimationRefresh::getRunningState() const
{
    return runningState;
}

void AnimationRefresh::setPad(int pad)
{
    if(pad >0){
        this->pad = pad;
    }
}

int AnimationRefresh::getPad() const
{
    return pad;
}

void AnimationRefresh::stop()
{
    if(!runningState){
        return;
    }
    aniController.stop();
    this->setVisible(false);
    runningState = false;
    emit stopped();
}

void AnimationRefresh::paintEvent(QPaintEvent *e)
{
    painter->begin(this);
//    painter->setRenderHint(QPainter::Antialiasing);
    painter->save();
    painter->setPen(cColor4);
    painter->setBrush(cColor4);
    foreach(QRect r, poses){
        painter->drawRect(r);
    }
    painter->restore();
    painter->save();
    painter->setPen(cColor1);
    painter->setBrush(cColor1);
    painter->drawRect(painterPos1);
    painter->restore();
    painter->save();
    painter->setPen(cColor2);
    painter->setBrush(cColor2);
    painter->drawRect(painterPos2);
    painter->restore();
    painter->save();
    painter->setPen(cColor3);
    painter->setBrush(cColor3);
    painter->drawRect(painterPos3);
    painter->restore();
    painter->end();
    e->accept();
}

void AnimationRefresh::initUi()
{
    this->setFixedSize(this->width(), this->height());
    widthi = this->width()/3;
    heighti = this->height()/3;
    if(pad == PAD_INITVALUE){
        pad = (this->width() + this->height())*0.05;
    }
    this->setFixedSize(widthi*3, heighti*3);
    r11.setRect(       0,         0, widthi-pad, heighti-pad);
    r12.setRect(  widthi,         0, widthi-pad, heighti-pad);
    r13.setRect(2*widthi,         0, widthi-pad, heighti-pad);
    r21.setRect(       0,   heighti, widthi-pad, heighti-pad);
    r23.setRect(2*widthi,   heighti, widthi-pad, heighti-pad);
    r31.setRect(       0, 2*heighti, widthi-pad, heighti-pad);
    r32.setRect(  widthi, 2*heighti, widthi-pad, heighti-pad);
    r33.setRect(2*widthi, 2*heighti, widthi-pad, heighti-pad);
    poses << r11 << r12 << r13 << r23 << r33 << r32 << r31 << r21;
}


