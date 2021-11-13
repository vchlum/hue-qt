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

#include "menuslider.h"

MenuSlider::MenuSlider(QWidget *parent) : QSlider(parent)
{
    setOrientation(Qt::Horizontal);
    setRange(0, 100);
    setValue(0);
    setMinimumWidth(200);
    setMaximumWidth(200);

    setColor("#b4b4b4");
}

void MenuSlider::setColor(QColor color)
{
    //setStyleSheet("QSlider::handle:horizontal {background-color: red;} QSlider::groove:horizontal {background-color:green;}");
    //setStyleSheet("QSlider::handle:horizontal {background-color: red;} QSlider::handle:vertical {background-color:green;}");

    //https://doc.qt.io/archives/qt-4.8/stylesheet-examples.html#customizing-qslider

    setStyleSheet("\
        QSlider {border-right: 8px solid transparent;}\
        QSlider::groove:horizontal {\
            border: 0px solid #999999;\
            height: 8px;\
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\
            margin: 2px 0;\
        }\
        \
        QSlider::handle:horizontal {\
            background: " + color.name() + ";\
            border: 0px solid #5c5c5c;\
            width: 18px;\
            margin: -2px 0;\
            border-radius: 3px;\
        }\
        \
        QSlider::sub-page:horizontal {\
            border: 0px solid #999999;\
            height: 8px;\
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " + color.name() + ", stop:1 #c4c4c4);\
            margin: 2px 0;\
        }\
        QSlider::add-page:horizontal {\
            border: 0px solid #999999;\
            height: 8px;\
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);\
            margin: 2px 0;\
        }"\
    );
}