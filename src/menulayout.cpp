/* Hue Lights 2 - Application for controlling Philips Hue Bridge and HDMI Syncbox
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

#include <QIcon>

#include "menulayout.h"
#include "menuslider.h"

MenuLayout::MenuLayout(bool use_slider, bool use_switch, QWidget *parent): QHBoxLayout(parent)
{
    label = new QLabel(); 
    icon = new QLabel();

    has_slider = use_slider;
    has_switch = use_switch;

    setAlignment(Qt::AlignLeft);

    addWidget(icon);
    setAlignment(icon, Qt::AlignLeft);

    if (has_slider) {
        auto slider_box = new QVBoxLayout();

        slider = new MenuSlider();

        slider_box->addWidget(label);
        slider_box->addWidget(slider);

        addLayout(slider_box);
    } else {
        addWidget(label);
    }

    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    if (has_switch) {
        mswitch = new MenuSwitch();
        addWidget(mswitch);
        setAlignment(mswitch, Qt::AlignRight);
    }

    setSpacing(0);
    setContentsMargins( 0, 0, 0, 0 );
}

void MenuLayout::setText(QString text)
{
    label->setText(text);
    label->setMinimumWidth(label->sizeHint().width());
}
void MenuLayout::setIcon(QString icon_name)
{
    QIcon tmp_icon (icon_name);

    icon->setPixmap(tmp_icon.pixmap(QSize(24, 24)));
}

void MenuLayout::setColor(QString color)
{
    if (has_slider) {
        slider->setColor(color);
    }

    if (has_switch) {
        mswitch->setColor(color);
    }
}
