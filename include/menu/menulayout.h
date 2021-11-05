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

#ifndef MENULAYOUT_H
#define MENULAYOUT_H

#include <QHBoxLayout>
#include <QLabel>

#include "menuswitch.h"
#include "menuslider.h"

class MenuLayout : public QHBoxLayout
{
    Q_OBJECT

    public:
        explicit MenuLayout(bool use_slider = false, bool use_switch = false, QWidget* parent = 0);
        void setText(QString text);
        void setIcon(QString icon_name);
        void setColor(QString color);

    protected:

    private:
        bool has_slider;
        bool has_switch;
        QLabel *label;
        QLabel *icon;
        MenuSlider *slider;
        MenuSwitch *mswitch;

    public slots:
};

#endif // MENULAYOUT_H