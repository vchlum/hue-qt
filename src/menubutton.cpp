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

        slider_box->addWidget(label);
        slider_box->addWidget(slider);

        for (int i =0; i < points; ++i) {
            slider_box->addWidget(new MenuSlider(this));
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
    }


    setLayout(button_layout);
    setStyleSheet("QPushButton {border: 10px solid transparent; text-align:left;}");
    setMinimumWidth(button_layout->sizeHint().width());
    setMinimumHeight(button_layout->sizeHint().height());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    adjustSize();
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

void MenuButton::setSwitch(bool on)
{
    if (!has_switch) {return;}
    button_switch->setValue(on);
}

void MenuButton::setSlider(int value)
{
    if (!has_slider) {return;}
    slider->setValue(value);
}

void MenuButton::setCombined(bool c)
{
    combined_state = c;
}

bool MenuButton::combined()
{
    return combined_state;
}

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent)
{}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}