#include "proxystyle.h"
#include <QRadialGradient>
#include <QDebug>

#define ARR_DRAW_POLYGON
//#define PUSHBUTTON_TEXT_BOLD

const int tabBarLabelWidth = 120;
const int tabBarLabelHeight = 60;
const int shadowWidth = 3;
const int button_up_height = 43;                            // tabwidget menu的高度
const QColor TabBar_background_color(35,35,35);             // tabwidget左侧选项卡的label的背景颜色，需要和qss里QTabWidget的背景颜色一致
const QColor TabWidget_menu_background_color(40,40,40);     // 一横条的背景，宽度是button_up_height + offset
const QColor PushButton_disable_color(40,40,40);            // disable color 要比上边这个menu background color 浅

QSize TabBarStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    QSize t = QProxyStyle::sizeFromContents(type, option, size, widget);
    if (type == QStyle::CT_TabBarTab) {
        t.transpose();
        t.rwidth() = tabBarLabelWidth;
        t.rheight() = tabBarLabelHeight;
    }
    return t;
}

void TabBarStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)
    if (element == CE_TabBarTabLabel) {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect labelRect = tab->rect;
            // 抗锯齿
            painter->setRenderHint(QPainter::Antialiasing);
            if (tab->state & State_Selected){
                painter->save();
                // 选中的tab bar 的边框颜色
                painter->setPen(QColor(55,57,55));
                // 选中的tab bar 的填充色
                painter->setBrush(QBrush(QColor(150,153,153)));
                // 选中的tab bar 的填充形状 （y+2）
                QPoint ps[5]= {labelRect.topLeft(),labelRect.topRight(),
                                QPoint((labelRect.left() + 4*labelRect.right())*0.2,(labelRect.top() + labelRect.bottom())*0.5+2),
                                labelRect.bottomRight(), labelRect.bottomLeft()};
                painter->drawPolygon(ps,5);
                painter->restore();
                painter->save();
                painter->setPen(QColor(71,71,71));
                painter->setBrush(QBrush(QColor(71,71,71)));
                QPoint shadowPoints[8] = {labelRect.topLeft(),labelRect.topRight(),
                                          QPoint(labelRect.topRight().x()    - shadowWidth, labelRect.topRight().y()    + shadowWidth),
                                          QPoint(labelRect.topLeft().x()     + shadowWidth, labelRect.topLeft().y()     + shadowWidth),
                                          QPoint(labelRect.bottomLeft().x()  + shadowWidth, labelRect.bottomLeft().y()  - shadowWidth),
                                          QPoint(labelRect.bottomRight().x() - shadowWidth, labelRect.bottomRight().y() - shadowWidth),
                                          labelRect.bottomRight(),
                                          labelRect.bottomLeft()};
                painter->drawPolygon(shadowPoints, 8);
                painter->restore();
            }
            else if(tab->state & State_MouseOver){
                // mouseover 在border会有颜色残留bug
                // 所以 setPen设置边缘为背景颜色
                painter->save();
                painter->setPen(TabBar_background_color);
                painter->setBrush(QColor(56,56,53));
                painter->drawRect(labelRect);
                painter->restore();
            }
            else{
                // 平时背景设为与背景颜色一致，透明pushbutton
                painter->save();
                painter->setPen(TabBar_background_color);
                painter->setBrush(TabBar_background_color);
                painter->drawRect(labelRect);
                painter->restore();
            }
            painter->save();
            QTextOption optionS;
            // 文字居中，默认文字方向就是水平的
            optionS.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            // selected 与 !selected的tab bar 的文字颜色
            (tab->state & QStyle::State_Selected) ? painter->setPen(QColor(40,41,42)) : painter->setPen(QColor(225,225,225));
            painter->drawText(labelRect.adjusted(int(labelRect.width()*0.382),0,0,0), tab->text, optionS);
            painter->restore();

            // 设置图标
            const int iconWidth = 30;
            const int iconHeight = 30;
            QRect iconRect(labelRect.topLeft().x() + int(labelRect.width()*0.382-iconWidth)-5
                           , labelRect.topLeft().y() + int(labelRect.height()*0.5-iconHeight*0.5)
                               , iconWidth,iconHeight);
            painter->save();
            if(tab->state & QStyle::State_Selected){
                painter->drawPixmap(iconRect, tab->icon.pixmap(QSize(iconWidth,iconHeight), QIcon::Mode::Selected));
            }
            else{
                painter->drawPixmap(iconRect, tab->icon.pixmap(QSize(iconWidth,iconHeight), QIcon::Mode::Normal));
            }
            painter->restore();
            return;
        }
    }
    if(element == CE_TabBarTab){
        QProxyStyle::drawControl(element,option,painter,widget);
    }
}


const int TabWidget_menu_horizontal_margin = 0;     // 一横条的高与pushbutton的高度差，要比pushbutton宽。
const int TabWidget_menu_vertical_margin = 5;     // 一横条的高与pushbutton的高度差，要比pushbutton宽。
void TabWidgetStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if(element == PE_FrameTabWidget){
        if(const QStyleOptionTabWidgetFrame *tableWF = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option)){
            QRect r=widget->rect();
            // TabWidget右侧border缺的6px涂色
            painter->setRenderHint(QPainter::Antialiasing);
            painter->save();
            painter->setPen(QColor(61,61,61));
            QPoint po[2] = {r.topRight(), r.bottomRight()};
            painter->drawPolyline(po,2);
            painter->restore();
            // QTabWidget背景，最上面那一排放按钮的地方的背景不要涂
            painter->save();
            // 从左到右的渐变
            //            QLinearGradient g(QPoint(r.topLeft().x()+tabBarLabelWidth-1, r.topLeft().y()+button_up_height+TabWidget_menu_horizontal_margin),
            //                              QPoint(r.topRight().x(), r.topRight().y()+button_up_height+TabWidget_menu_horizontal_margin));
            //            g.setColorAt(0,QColor(51,51,51));
            //            g.setColorAt(1,QColor(61,61,61));
            painter->setPen(QColor(51,51,51));
            painter->setBrush(QColor(51,51,51));
            painter->drawRect(r.adjusted(tabBarLabelWidth, button_up_height+TabWidget_menu_horizontal_margin,0,0));
            painter->restore();
            // QTabWidget最上面那一排放按钮的地方的背景
            painter->save();
            painter->setPen(TabWidget_menu_background_color);
            painter->setBrush(TabWidget_menu_background_color);
            painter->drawRect(r.adjusted(tabBarLabelWidth,0,0, button_up_height+TabWidget_menu_horizontal_margin-r.height()));
            painter->restore();
            return;
        }
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);

}

QRect TabWidgetStyle::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect rect  = QProxyStyle::subElementRect(element, option, widget);
    if(element == SE_TabWidgetTabBar){
        // 把tabbar向下移一段距离，这段距离的长度是顶端要显示的那一行按钮的高度 + offset_width
        rect.adjust(0,button_up_height+TabWidget_menu_horizontal_margin,0, button_up_height+TabWidget_menu_horizontal_margin);
    }
    return rect;
}

void TabWidgetStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    QProxyStyle::drawControl(element, option, painter, widget);
}

const int icon_offset = 7;                  // 图标左侧和border的距离
const int text_offset = 3;                  // 文字左侧和图标的距离
const int pushButton_iconWidth = 30;        // 图标的长度和宽度
const int pushButton_iconHeight = 30;
void PushButtonStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)
    if(element == CE_PushButton){
        if(const QStyleOptionButton *pb = qstyleoption_cast<const QStyleOptionButton *>(option)){
            QRect labelRect = pb->rect;
            // 抗锯齿
            painter->setRenderHint(QPainter::Antialiasing);
            if(pb->state & State_Enabled){
                // pressed
                if(pb->state & State_Sunken){
                    // 背景颜色
                    painter->save();
                    painter->setPen(QPen(QColor(55,85,100), 2));
                    painter->setBrush(QColor(51,51,51));
                    painter->drawRect(labelRect);
                    painter->restore();
                }
                // hover
                else if(pb->state & State_MouseOver){
                    // 背景颜色
                    painter->save();
                    painter->setPen(QPen(QColor(55,85,100), 2));
                    painter->setBrush(QColor(64,64,61));
                    painter->drawRect(labelRect);
                    painter->restore();
                }
                // 一般情况
                else{
                    // 背景颜色
                    painter->save();
                    painter->setPen(TabWidget_menu_background_color);
                    painter->setBrush(TabWidget_menu_background_color);
                    painter->drawRect(labelRect);
                    painter->restore();
                }
                // 文字颜色
                painter->save();
#ifdef PUSHBUTTON_TEXT_BOLD
                QFont f = painter->font();
                f.setBold(true);
                painter->setFont(f);
#endif
                QTextOption optionS;
                optionS.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                painter->setPen(QColor(240,240,240));
                painter->drawText(labelRect.adjusted(icon_offset+pushButton_iconWidth+text_offset,0,0,0), pb->text, optionS);
                painter->restore();
            }
            // diabled
            else{
                painter->save();
                painter->setPen(PushButton_disable_color);
                painter->setBrush(PushButton_disable_color);
                painter->drawRect(labelRect);
                painter->restore();

                // disabled 文字颜色
                painter->save();
#ifdef PUSHBUTTON_TEXT_BOLD
                QFont f = painter->font();
                f.setBold(true);
                painter->setFont(f);
#endif
                QTextOption optionS;
                optionS.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                painter->setPen(Qt::gray);
                painter->drawText(labelRect.adjusted(icon_offset+pushButton_iconWidth+text_offset,0,0,0), pb->text, optionS);
                painter->restore();


            }

            // 设置图标
            painter->save();
            QRect iconRect(labelRect.topLeft().x()+icon_offset
                           , labelRect.topLeft().y()+TabWidget_menu_vertical_margin
                           , pushButton_iconWidth,pushButton_iconHeight);
            painter->drawPixmap(iconRect, pb->icon.pixmap(QSize(pushButton_iconWidth, pushButton_iconHeight), QIcon::Mode::Normal));
            painter->restore();

            return;
        }
    }

    QProxyStyle::drawControl(element, option, painter, widget);

}

const int padding_up = 4;
const int padding_left = 4;
const int padding_down = 4;

void HorizontalScrollBarStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if(control == CC_ScrollBar){
        if(const QStyleOptionSlider *sli = qstyleoption_cast<const QStyleOptionSlider *>(option)){
            QRect sliRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
            QRect grooveRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarGroove, widget);
            QRect upRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubLine, widget);
            QRect downRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddLine, widget);
            // 抗锯齿
            painter->setRenderHint(QPainter::Antialiasing);
            painter->save();
            painter->setPen(QColor(201,204,204));
            painter->setBrush(QColor(201,204,204));
            QPoint addGround[4] = {grooveRect.topLeft(), grooveRect.topRight(), grooveRect.bottomRight(), grooveRect.bottomLeft()};
            painter->drawPolygon(addGround,4);
            painter->restore();
            // ScrollBar的slider(handle)
            if(sli->state & QStyle::State_Sunken){
                painter->save();
                painter->setPen(QColor(51,51,51));
                QLinearGradient g(sliRect.topRight(), sliRect.bottomRight());
                g.setColorAt(0,QColor(51,51,51));
                g.setColorAt(1,QColor(61,61,61));
                painter->setBrush(g);
                QPoint backGround[4] = {sliRect.topLeft(), sliRect.topRight(), sliRect.bottomRight(), sliRect.bottomLeft()};
                painter->drawPolygon(backGround,4);
                painter->restore();
            }
            else if(sli->state & QStyle::State_MouseOver){
                painter->save();
                painter->setPen(QColor(85,85,90));
                QLinearGradient g(sliRect.topRight(), sliRect.bottomRight());
                g.setColorAt(0,QColor(85,85,90));
                g.setColorAt(1,QColor(95,100,100));
                painter->setBrush(g);
                QPoint backGround[4] = {sliRect.topLeft(), sliRect.topRight(), sliRect.bottomRight(), sliRect.bottomLeft()};
                painter->drawPolygon(backGround,4);
                painter->restore();
            }
            else{
                painter->save();
                painter->setPen(QColor(120,125,130));
                QLinearGradient g(sliRect.topRight(), sliRect.bottomRight());
                g.setColorAt(0,QColor(120,125,130));
                g.setColorAt(1,QColor(132,135,140));
                painter->setBrush(g);
                QPoint backGround[4] = {sliRect.topLeft(), sliRect.topRight(), sliRect.bottomRight(), sliRect.bottomLeft()};
                painter->drawPolygon(backGround,4);
                painter->restore();
            }
            // 两个arrow所在的方框
            painter->save();
            painter->setPen(QColor(51,51,51));
            painter->setBrush(QColor(240,255,255));
            QPoint up[4] = {upRect.topLeft(), upRect.topRight(), upRect.bottomRight(), upRect.bottomLeft()};
            QPoint down[4] = {downRect.topLeft(), downRect.topRight(), downRect.bottomRight(), downRect.bottomLeft()};
            painter->drawPolygon(up,4);
            painter->drawPolygon(down,4);
            painter->restore();

            // arrow的颜色和形状
            painter->save();
            painter->setPen(QColor(240,255,255));
            painter->setBrush(QColor(51,51,51));
            int topRightY = upRect.bottomLeft().y();
            int topwidth = upRect.width();
            int downRightX = downRect.x();
            QPoint leftArr[3] = {
                QPoint(padding_up, topRightY*0.5),
                QPoint(topwidth-padding_down, padding_left),
                QPoint(topwidth-padding_down, topRightY-padding_left),
                };
            QPoint rightArr[3] = {
                QPoint(padding_down+downRightX, padding_left),
                QPoint(padding_down+downRightX, topRightY-padding_left),
                QPoint(topwidth-padding_down+downRightX, topwidth*0.5),
                };
            painter->drawPolygon(leftArr,3);
            painter->drawPolygon(rightArr,3);
            painter->restore();
            return;
        }
    }
    else{
        drawComplexControl(control, option, painter, widget);
    }
}



void VerticalScrollBarStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if(control == CC_ScrollBar){
        if(const QStyleOptionSlider *sli = qstyleoption_cast<const QStyleOptionSlider *>(option)){
            QRect sliRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
            QRect grooveRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarGroove, widget);
            QRect upRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubLine, widget);
            QRect downRect = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddLine, widget);
            // 抗锯齿
            painter->setRenderHint(QPainter::Antialiasing);
            painter->save();
            painter->setPen(QColor(201,204,204));
            painter->setBrush(QColor(201,204,204));
            QPoint addGround[4] = {grooveRect.topLeft(), grooveRect.topRight(), grooveRect.bottomRight(), grooveRect.bottomLeft()};
            painter->drawPolygon(addGround,4);
            painter->restore();
            // ScrollBar的slider(handle)
            if(sli->state & QStyle::State_Sunken){
                painter->save();
                painter->setPen(QColor(51,51,51));
                QLinearGradient g(sliRect.topRight(), sliRect.bottomRight());
                g.setColorAt(0,QColor(51,51,51));
                g.setColorAt(1,QColor(61,61,61));
                painter->setBrush(g);
                QPoint backGround[4] = {sliRect.topLeft(), sliRect.topRight(), sliRect.bottomRight(), sliRect.bottomLeft()};
                painter->drawPolygon(backGround,4);
                painter->restore();
            }
            else if(sli->state & QStyle::State_MouseOver){
                painter->save();
                painter->setPen(QColor(85,85,90));
                QLinearGradient g(sliRect.topRight(), sliRect.bottomRight());
                g.setColorAt(0,QColor(85,85,90));
                g.setColorAt(1,QColor(95,100,100));
                painter->setBrush(g);
                QPoint backGround[4] = {sliRect.topLeft(), sliRect.topRight(), sliRect.bottomRight(), sliRect.bottomLeft()};
                painter->drawPolygon(backGround,4);
                painter->restore();
            }
            else{
                painter->save();
                painter->setPen(QColor(120,125,130));
                QLinearGradient g(sliRect.topRight(), sliRect.bottomRight());
                g.setColorAt(0,QColor(120,125,130));
                g.setColorAt(1,QColor(132,135,140));
                painter->setBrush(g);
                QPoint backGround[4] = {sliRect.topLeft(), sliRect.topRight(), sliRect.bottomRight(), sliRect.bottomLeft()};
                painter->drawPolygon(backGround,4);
                painter->restore();
            }
            // 两个arrow所在的方框
            painter->save();
            painter->setPen(QColor(51,51,51));
            painter->setBrush(QColor(240,255,255));
            QPoint up[4] = {upRect.topLeft(), upRect.topRight(), upRect.bottomRight(), upRect.bottomLeft()};
            QPoint down[4] = {downRect.topLeft(), downRect.topRight(), downRect.bottomRight(), downRect.bottomLeft()};
            painter->drawPolygon(up,4);
            painter->drawPolygon(down,4);
            painter->restore();

            // arrow的颜色和形状
            painter->save();
            painter->setPen(QColor(240,255,255));
            painter->setBrush(QColor(51,51,51));
            int topRightX = upRect.topRight().x();
            int topHeight = upRect.height();
            int downRightY = downRect.y();
            QPoint upArr[3] = {
                QPoint(topRightX*0.5, padding_up),
                QPoint(padding_left, topHeight-padding_down),
                QPoint(topRightX-padding_left, topHeight-padding_down),
                };
            QPoint downArr[3] = {
                QPoint(padding_left, padding_down+downRightY),
                QPoint(topRightX-padding_left, padding_down+downRightY),
                QPoint(topHeight*0.5, topHeight-padding_down+downRightY),
                };
            painter->drawPolygon(upArr,3);
            painter->drawPolygon(downArr,3);
            painter->restore();
            return;
        }
    }
    else{
        drawComplexControl(control, option, painter, widget);
    }

}

void CheckBoxStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget);
    if(element == PE_IndicatorCheckBox){
        if(const QStyleOptionButton *cb = qstyleoption_cast<const QStyleOptionButton *>(option)){
            QRect iRect= cb->rect;
            if(cb->state & State_On){
                // 圆形渐变
                painter->setRenderHint(QPainter::Antialiasing);
                QRadialGradient indicatorColorGradient(QPointF(iRect.topLeft().x()*0.5 + iRect.bottomRight().x()*0.5, iRect.topLeft().y()*0.5 + iRect.bottomRight().y()*0.5),
                                                       15,
                                                       QPointF(iRect.topLeft().x()*0.5 + iRect.bottomRight().x()*0.5, iRect.topLeft().y()*0.5 + iRect.bottomRight().y()*0.5));
                indicatorColorGradient.setColorAt(0, QColor(40,65,90));
                indicatorColorGradient.setColorAt(0.9, QColor(115,120,140));
                indicatorColorGradient.setColorAt(1, QColor(240,255,255));
                painter->save();
                painter->setPen(QColor(115,120,140));
                painter->setBrush(QBrush(indicatorColorGradient));
                painter->drawRect(iRect);
                painter->restore();
            }
            else{
                painter->save();
                painter->setPen(QColor(115,120,140));
                painter->setBrush(QBrush(QColor(240,255,255)));
                painter->drawRect(iRect);
                painter->restore();
            }
        }
    }
    return;
}

void RadioButtonStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget);
    if(element == PE_IndicatorRadioButton){
        if(const QStyleOptionButton *rb = qstyleoption_cast<const QStyleOptionButton *>(option)){
            QRect rRect = rb->rect;
            if(rb->state & State_On){
                // 圆形渐变
                painter->setRenderHint(QPainter::Antialiasing);
                QRadialGradient indicatorColorGradient(QPointF(rRect.topLeft().x()*0.5 + rRect.bottomRight().x()*0.5, rRect.topLeft().y()*0.5 + rRect.bottomRight().y()*0.5),
                                                       15,
                                                       QPointF(rRect.topLeft().x()*0.5 + rRect.bottomRight().x()*0.5, rRect.topLeft().y()*0.5 + rRect.bottomRight().y()*0.5));
                indicatorColorGradient.setColorAt(0, QColor(40,65,90));
                indicatorColorGradient.setColorAt(0.9, QColor(115,120,140));
                indicatorColorGradient.setColorAt(1, QColor(240,255,255));
                painter->save();
                painter->setPen(QColor(115,120,140));
                painter->setBrush(QBrush(indicatorColorGradient));
                painter->drawRoundedRect(rRect, rRect.width()*0.5, rRect.height()*0.5);
                painter->restore();
            }
            else{
                painter->save();
                painter->setPen(QColor(115,120,140));
                painter->setBrush(QBrush(QColor(240,255,255)));
                painter->drawRoundedRect(rRect, rRect.width()*0.5, rRect.height()*0.5);
                painter->restore();
            }
        }
    }
    return;
}
