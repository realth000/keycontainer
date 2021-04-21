#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>
#include <QStyleOption>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionTab>
#include <QStyleOptionTabWidgetFrame>
#include <QStyleOptionButton>
#include <QStyledItemDelegate>
#include <QStyleHintReturn>

class TabBarStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,const QSize &size, const QWidget *widget) const override;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
};

class TabWidgetStyle : public QProxyStyle
{
public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};

class PushButtonStyle : public QProxyStyle
{
public:
    PushButtonStyle(QString borderColorNormal = "");
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

private:
    const QString normalColor;
};

class HorizontalScrollBarStyle : public QProxyStyle
{
public:
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

};

class VerticalScrollBarStyle : public QProxyStyle
{
public:
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

};

class CheckBoxStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};

class RadioButtonStyle : public QProxyStyle
{
public:
    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
};

#endif // PROXYSTYLE_H
