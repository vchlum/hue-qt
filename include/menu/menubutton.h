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

#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QIcon>
#include <QLabel>

#include "menuswitch.h"
#include "menuslider.h"

class MenuButton : public QPushButton
{
    Q_OBJECT
    private:
        QString identifier;
        bool has_slider;
        bool has_switch;
        bool all_items = false;
        QLabel *label;
        QLabel *icon;
        MenuSlider *slider;
        MenuSwitch *button_switch;

    public slots:

    signals:
        void buttonRemoved(QString s);
    public:
        explicit MenuButton(QString item_id, bool use_slider, bool use_switch, QWidget *parent = 0);
        ~MenuButton() { emit buttonRemoved(id()); }
        QString id();
        void setText(QString text);
        void setIcon(QString icon_name);
        void setColor(QColor color);
        void setSwitch(bool on);
        void setSlider(int value);
        void setAllItems(bool all);
        bool allItems();
};

#endif // MENUBUTTON_H