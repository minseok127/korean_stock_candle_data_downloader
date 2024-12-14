#ifndef SHINEBUTTON_H
#define SHINEBUTTON_H

#include <QPushButton>
#include <QColor>
#include <QPropertyAnimation>

class ShineButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)

public:
    explicit ShineButton(QPushButton *parent = nullptr);

    void SetColor(const QColor& color);
    const QColor& GetColor() const;

    void setFont(QString font);

    void set_info(double ratio, QString name = "");

protected:
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    QColor currentColor;

    QPropertyAnimation colorAnimation;

    QString font;

    QPushButton* center;
};

#endif // SHINEBUTTON_H
