/* Hue-QT - Application for controlling Philips Hue Bridge and HDMI Syncbox
 * Copyright (C) 2021 Václav Chlumský
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QHBoxLayout>

#include "menubutton.h"
#include "menuutils.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent)
{}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}

MenuButton::MenuButton(QString item_id, bool use_slider, int points, bool use_back, bool use_switch, QWidget *parent): QPushButton(parent)
{
    QHBoxLayout *button_layout = new QHBoxLayout(this);

    button_layout->setSpacing(0);
    button_layout->setContentsMargins( 0, 0, 0, 0 );

    label = new QLabel(this); 
    icon = new QLabel(this);

    identifier = item_id;
    has_slider = use_slider;
    has_switch = use_switch;
    combined_state = false;

    button_layout->setAlignment(Qt::AlignLeft);
    button_layout->setContentsMargins(0, 0, 0, 0);

    button_layout->addWidget(icon);
    button_layout->setAlignment(icon, Qt::AlignLeft);

    if (has_slider) {
        auto *slider_box = new QVBoxLayout();

        slider = new MenuSlider(this);
        connect(slider, &MenuSlider::valueChanged, [this](int value){
            if (!manual_set)
                emit dimmed(identifier, value);
        });

        slider_box->addWidget(label);
        slider_box->addWidget(slider);

        if (points > 0) {
            gradient_points.resize(points);

            auto *gardient_box = new QHBoxLayout();

            for (int i = 0; i < points; ++i) {
                ClickableLabel *point = new ClickableLabel(this);

                QPixmap pixmap = getColorPixmapFromSVG(":images/HueIcons/uicontrolsColorScenes.svg", QColor("#2E2E2E"));

                point->setPixmap(pixmap.scaled(points_icon_size, points_icon_size, Qt::KeepAspectRatio));
                gardient_box->addWidget(point);
                gradient_points[i] = point;
            }

            slider_box->addLayout(gardient_box);
        }

        button_layout->addLayout(slider_box);
    } else {
        button_layout->addWidget(label);
    }

    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    if (use_back) {
        ClickableLabel* back_icon = new ClickableLabel(this);
        QIcon tmp_icon (":images/HueIcons/settingsSoftwareUpdate.svg");
        back_icon->setPixmap(tmp_icon.pixmap(QSize(20, 20)));
        button_layout->addWidget(back_icon);
        connect(back_icon, &ClickableLabel::clicked, [this]() { emit goBack();});
    }

    if (has_switch) {
        button_switch = new MenuSwitch(this);
        button_layout->addWidget(button_switch);
        button_layout->setAlignment(button_switch, Qt::AlignRight);
        connect(button_switch, &MenuSwitch::clicked, [this]() {
            emit switched(identifier, button_switch->value());
        });
    }

    setLayout(button_layout);
    setStyleSheet(QString("QPushButton {border: %1px solid transparent; text-align:left;}").arg(my_border));
    setMinimumWidth(button_layout->sizeHint().width());
    setMinimumHeight(button_layout->sizeHint().height());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    adjustSize();
}

QSize MenuButton::sizeHint() const {
    int my_width;
    int my_height;

    my_height = label->sizeHint().height() + 2 * my_border;
    if (has_slider)
        my_height += slider->sizeHint().height();

    if (gradient_points.size() > 0)
        my_height += points_icon_size;
    my_width = width();


    return QSize(my_width, my_height);
}

QString MenuButton::id()
{
    return identifier;
}

void MenuButton::setText(QString text)
{
    label->setText(text);
    label->setMinimumWidth(label->sizeHint().width());
}

void MenuButton::setIcon(QString icon_name)
{
    QIcon tmp_icon (icon_name);

    icon->setPixmap(tmp_icon.pixmap(QSize(24, 24)));
}

void MenuButton::setColor(QColor color)
{
    if (has_slider) {
        slider->setColor(color);
    }

    if (has_switch) {
        button_switch->setColor(color);
    }
}

void MenuButton::setColorGradients(QVarLengthArray<QColor> colors)
{
    if (gradient_points.length() == 0) {
        return;
    }

    if (gradient_points.length() != colors.length()) {
        return;
    }

    for (int i = 0; i < colors.length(); ++i) {
        QPixmap pixmap = getColorPixmapFromSVG(":images/HueIcons/uicontrolsColorScenes.svg", colors[i]);
        gradient_points[i]->setPixmap(pixmap.scaled(24, 24, Qt::KeepAspectRatio));
    }
}

void MenuButton::setSwitch(bool on)
{
    if (!has_switch) {return;}

    button_switch->setValue(on);
}

void MenuButton::setSlider(int value)
{
    if (!has_slider) {return;}

    manual_set = true;
    slider->setValue(value);
    manual_set = false;
}

void MenuButton::setCombined(bool c)
{
    combined_state = c;
}

bool MenuButton::combined()
{
    return combined_state;
}
