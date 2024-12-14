#include "shinebutton.h"
#include <QEvent>
#include <QDebug>

ShineButton::ShineButton(QPushButton *parent) :
    QPushButton(parent),
    colorAnimation(this, "color")
{
}

void ShineButton::SetColor(const QColor& color)
{
    currentColor = color;
    QString css = "border-radius: 80px; ";
    css.append("border: 3px solid rgb(61,185,127); ");
    QString strColor = QString("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
    css.append("background-color: " + strColor + "; ");
    css.append(font);
    setStyleSheet(css);
}

const QColor& ShineButton::GetColor() const
{
    return currentColor;
}

void ShineButton::setFont(QString font){
    this->font = font;
}

void ShineButton::enterEvent(QEvent*)
{
    colorAnimation.stop();

    colorAnimation.setDuration(500); //set your transition
    colorAnimation.setStartValue(GetColor()); //starts from current color
    colorAnimation.setEndValue(QColor(61,185,127));//set your color

    colorAnimation.setEasingCurve(QEasingCurve::Linear);//animation style

    colorAnimation.start();
}

void ShineButton::leaveEvent(QEvent*)
{
    colorAnimation.stop();

    colorAnimation.setDuration(500); //set your transition
    colorAnimation.setStartValue(GetColor()); //starts from current color
    colorAnimation.setEndValue(QColor(0, 0, 0));//set your color

    colorAnimation.setEasingCurve(QEasingCurve::Linear);//animation style

    colorAnimation.start();
}
