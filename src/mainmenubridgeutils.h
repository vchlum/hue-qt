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

#ifndef MAINMENUBRIDGEUTILS_H
#define MAINMENUBRIDGEUTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include <QColor>

struct ItemState {
    bool has_on = false;
    bool on = false;
    bool on_any = false;
    bool on_all = false;

    bool has_dimming = false;
    int brightness = 0;

    bool has_color = false;
    QColor color;

    bool has_color_temperature = false;
    int color_temperature = 0;
    int color_temperature_minimum = 0;
    int color_temperature_maximum = 0;

    QVarLengthArray<QString> ids;
} ;

QJsonObject getItemById(QJsonObject json, QString id);
QString getTypeById(QJsonObject json, QString id);
QJsonArray getItemsByType(QJsonObject json, QString type);
QJsonArray getServicesById(QJsonObject json, QString id);

ItemState getLightState(QJsonObject json, QString id);
ItemState getStateById(QJsonObject json, QString id);

/*
 Convert xy and brightness to RGB
 https://stackoverflow.com/questions/22894498/philips-hue-convert-xy-from-api-to-hex-or-rgb
 https://stackoverflow.com/questions/16052933/convert-philips-hue-xy-values-to-hex
*/
QColor XYBriToColor(double x, double y, int bri);

#endif // MAINMENUBRIDGEUTILS_H