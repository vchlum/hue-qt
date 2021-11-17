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

#ifndef MAINMENUBRIDGEUTILS_H
#define MAINMENUBRIDGEUTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include <QColor>

#include <menubutton.h>

struct ItemState {
    bool dummy = false; // true -> this state is not useful

    QString id = "";
    QString type = "";
    QString name = "";
    QString archetype = "";

    bool has_on = false;
    bool on = false;

    bool has_dimming = false;
    int brightness = 0;

    bool has_color = false;
    QColor color;

    bool has_color_temperature = false;
    int color_temperature = 0;
    int color_temperature_minimum = 0;
    int color_temperature_maximum = 0;

    bool has_gradient = false;
    int gradient_points_capable = 0;
    QVarLengthArray<QColor> gradient_points;

    QMap<QString, QString> services; // <rid, rtype>

    QString group_id = ""; // scene affiliation
    QString group_type = "";

    QVarLengthArray<MenuButton*> items;
};

ItemState getLightFromJson(QJsonObject json);
ItemState getGroupFromJson(QJsonObject json);
ItemState getSceneFromJson(QJsonObject json);

ItemState updateState(ItemState state, QJsonObject json);
ItemState combineTwoStates(ItemState base, ItemState joiner);

/*
 Convert xy and brightness to RGB
 https://stackoverflow.com/questions/22894498/philips-hue-convert-xy-from-api-to-hex-or-rgb
 https://stackoverflow.com/questions/16052933/convert-philips-hue-xy-values-to-hex
*/
QColor XYBriToColor(double x, double y, int bri);

#endif // MAINMENUBRIDGEUTILS_H