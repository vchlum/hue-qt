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

#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QIcon>
#include <QLabel>

#include "menuswitch.h"
#include "menuslider.h"

class ClickableLabel : public QLabel
{
    Q_OBJECT

    public:
        explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
        ~ClickableLabel();

    signals:
        void clicked();

    protected:
        void mousePressEvent(QMouseEvent* event);
};

class MenuButton : public QPushButton
{
    Q_OBJECT
    private:
        QString identifier;
        bool has_slider;
        bool has_switch;
        QLabel *label;
        QLabel *icon;
        MenuSlider *slider;
        MenuSwitch *button_switch;
        bool combined_state;
        QVarLengthArray<ClickableLabel*> gradient_points;
        bool manual_set = false;

    public slots:

    signals:
        void switched(QString id, bool on);
        void dimmed(QString id, int value);
        void buttonRemoved(QString s);
        void goBack();

    public:
        explicit MenuButton(QString item_id, bool use_slider, int points, bool use_back, bool use_switch, QWidget *parent = 0);
        ~MenuButton() { emit buttonRemoved(id()); }
        QString id();
        void setText(QString text);
        void setIcon(QString icon_name);
        void setColor(QColor color);
        void setColorGradients(QVarLengthArray<QColor> colors);
        void setSwitch(bool on);
        void setSlider(int value);
        void setCombined(bool all);
        bool combined();
};

#endif // MENUBUTTON_H