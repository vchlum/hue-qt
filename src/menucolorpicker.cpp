#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QPalette>
#include <QLabel>
#include <QVector2D>

#include "menucolorpicker.h"

ColorWheel::ColorWheel(QWidget *parent): QWidget(parent)
{
    color = new QColor(255, 255, 255, 1);
    h = color->hueF();
    s = color->saturationF();
    v = color->valueF();
    x = 0.5;
    y = 0.5;
    margin = 10;
    radius = 0;
}

void ColorWheel::paintEvent(QPaintEvent *e)
{
    (void) e;

    QPointF center = QPointF(width()/2, height()/2);
    QPainter painter(this);

    painter.setViewport(margin, margin, width() - 2 * margin, height() - 2 * margin);

    QConicalGradient hsv_grad = QConicalGradient(center, 90);
    for (int i = 0; i < 360; ++i) {
        QColor col = QColor::fromHsvF(i / 360.0, 1, v);
        hsv_grad.setColorAt(i / 360.0, col);
    }

    QRadialGradient val_grad = QRadialGradient(center, radius);
    val_grad.setColorAt(0.0, QColor::fromHsvF(0.0, 0.0, v, 1.0));
    val_grad.setColorAt(1.0, Qt::transparent);

    painter.setPen(Qt::transparent);
    painter.setBrush(hsv_grad);
    painter.drawEllipse(rect());
    painter.setBrush(val_grad);
    painter.drawEllipse(rect());
}

void ColorWheel::resizeEvent(QResizeEvent *e)
{
    (void) e;

    radius = qMin(width() / 2, height() / 2);
}

QSize ColorWheel::sizeHint() const {
    return QSize(width(), height());
}

QColor ColorWheel::mapColor(int _x, int _y)
{
    h = (qAtan2(_x - radius, _y - radius) + M_PI)/(2.0 * M_PI);
    s = qSqrt(qPow(_x - radius, 2.0) + qPow(_y - radius, 2.0)) / radius;

    if (s > 1.0)
        s = 1.0;

    return QColor::fromHsvF(h, s, v);
}

void ColorWheel::mousePressEvent(QMouseEvent *e)
{
    emit colorPicked(mapColor(e->pos().x(), e->pos().y()));

    QWidget::mousePressEvent(e);
}

TemperatureBox::TemperatureBox(QWidget *parent): QWidget(parent)
{
    color = new QColor(255, 255, 255, 1);
    mirek_maximum = 500;
    mirek_minimum = 153;
    mirek_size = mirek_maximum - mirek_minimum;
    h = color->hueF();
    s = color->saturationF();
    v = color->valueF();

    margin = 10;
    radius = 0;
}

void TemperatureBox::paintEvent(QPaintEvent *e)
{
    (void) e;

    //QPointF center = QPointF(width()/2, height()/2);
    QPainter painter(this);

    painter.setViewport(margin, margin, width() - 2 * margin, height() - 2 * margin);

    QLinearGradient hsv_grad = QLinearGradient(0,0, width(), height());

    QGradientStops stops;
    stops << QGradientStop(0.0, QColor::fromRgb(255,147,44));
    stops << QGradientStop(1.0, QColor::fromRgb(211,224,255));

    hsv_grad.setStops(stops);

    painter.setPen(Qt::transparent);
    painter.setBrush(hsv_grad);
    painter.drawRoundedRect(rect(), 0.5, 0.5);
}

void TemperatureBox::resizeEvent(QResizeEvent *e)
{
    (void) e;

    radius = qMin(width() / 2, height() / 2);
}

QSize TemperatureBox::sizeHint() const {
    return QSize(width(), height());
}

void TemperatureBox::mousePressEvent(QMouseEvent *e)
{
    int mirek = mirek_maximum - e->pos().x() * mirek_size / (width() - margin);
    emit mirekPicked(mirek);

    QWidget::mousePressEvent(e);
}

ColorPicker::ColorPicker(QString id, bool use_color, bool use_temperature, QWidget *parent): QWidget(parent)
{
    identifier = id;
    main_layout = new QVBoxLayout(this);

    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);

    main_layout->setAlignment(Qt::AlignCenter);

    if (use_color) {
        ColorWheel* color_wheel = new ColorWheel(this);
        color_wheel->setContentsMargins(0, 0, 0, 0);

        color_wheel->setMinimumHeight(size_helper);
        color_wheel->setMinimumWidth(size_helper);
        color_wheel->setMaximumHeight(size_helper);
        color_wheel->setMaximumWidth(size_helper);

        connect(color_wheel, &ColorWheel::colorPicked, [this] (QColor color) {
            emit colorPicked(identifier, color);
        });

        main_layout->addWidget(color_wheel);

        width_helper = size_helper;
        height_helper += size_helper + 1;
    }

    if (use_temperature) {
        TemperatureBox* temperature_box = new TemperatureBox(this);
        temperature_box->setContentsMargins(0, 0, 0, 0);

        temperature_box->setMinimumHeight(size_helper / 4);
        temperature_box->setMinimumWidth(size_helper);
        temperature_box->setMaximumHeight(size_helper / 4);
        temperature_box->setMaximumWidth(size_helper);

        connect(temperature_box, &TemperatureBox::mirekPicked, [this] (int mirek) {
            emit mirekPicked(identifier, mirek);
        });

        main_layout->addWidget(temperature_box);

        width_helper = size_helper;
        height_helper += (size_helper / 4) + 1;
    }
}

QSize ColorPicker::sizeHint() const {
    return QSize(width_helper, height_helper);
}
