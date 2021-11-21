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
#include <QCoreApplication>
#include <QEventLoop>

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
    QColor color = QColor(255, 255, 255);

    bool has_mirek = false;
    int mirek_temperature = 0;
    int mirek_min = 0;
    int mirek_max = 0;
    QColor mirek_color = QColor(255, 255, 255);

    bool has_gradient = false;
    int gradient_points_capable = 0;
    QVarLengthArray<QColor> gradient_points;

    QMap<QString, QString> services; // <rid, rtype>

    QString grouped_light_rid = "";

    QString group_id = ""; // scene affiliation
    QString group_type = "";

    QVarLengthArray<MenuButton*> items;
};

ItemState getLightFromJson(QJsonObject json);
ItemState getGroupFromJson(QJsonObject json);
ItemState getSceneFromJson(QJsonObject json);

ItemState updateState(ItemState state, QJsonObject json);
bool colorIsBlack(QColor color);
QColor combineTwoColors(QColor base, QColor joiner);
ItemState combineTwoStates(ItemState base, ItemState joiner);

void delay(int msec);

/*
 Convert xy and brightness to RGB
 https://stackoverflow.com/questions/22894498/philips-hue-convert-xy-from-api-to-hex-or-rgb
 https://stackoverflow.com/questions/16052933/convert-philips-hue-xy-values-to-hex
*/
QColor XYBriToColor(double x, double y, int bri);

/**
 * Converts RGB to xy values for Philips Hue Lights.
 * https://stackoverflow.com/questions/22564187/rgb-to-philips-hue-hsb
 * https://github.com/PhilipsHue/PhilipsHueSDK-iOS-OSX/commit/f41091cf671e13fe8c32fcced12604cd31cceaf3
 * https://developers.meethue.com/develop/application-design-guidance/color-conversion-formulas-rgb-to-xy-and-back/#Color-rgb-to-xy
 */
QVarLengthArray<float> colorToHueXY(QColor color);

/**
 * Converts kelvin temperature to RGB
 * https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
 */
QColor kelvinToColor(int kelvin);

int mirekToKelvin(int mirek);

#endif // MAINMENUBRIDGEUTILS_H