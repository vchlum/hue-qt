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

#include "mainmenubridgeutils.h"

ItemState getLightFromJson(QJsonObject json)
{
    ItemState state;
    state.id = json["id"].toString();
    state.type = json["type"].toString();

    if (json.contains("metadata")) {
        state.name = json["metadata"].toObject()["name"].toString();
        state.archetype = json["metadata"].toObject()["archetype"].toString();
    }

    return updateState(state, json);
}

ItemState getDeviceFromJson(QJsonObject json)
{
    ItemState state;
    state.id = json["id"].toString();
    state.type = json["type"].toString();

    if (json.contains("metadata")) {
        state.name = json["metadata"].toObject()["name"].toString();
        state.archetype = json["metadata"].toObject()["archetype"].toString();
    }

    return updateState(state, json);
}

ItemState getGroupFromJson(QJsonObject json)
{
    ItemState state;
    state.id = json["id"].toString();
    state.type = json["type"].toString();

    if (json.contains("metadata")) {
        state.name = json["metadata"].toObject()["name"].toString();
        state.archetype = json["metadata"].toObject()["archetype"].toString();
    }

    return updateState(state, json);
}

ItemState getSceneFromJson(QJsonObject json)
{
    ItemState state;
    state.id = json["id"].toString();
    state.type = json["type"].toString();

    if (json.contains("metadata")) {
        state.name = json["metadata"].toObject()["name"].toString();
    }

    return updateState(state, json);
}

ItemState updateState(ItemState state, QJsonObject json)
{
    double x;
    double y;

    if (json.contains("on")) {
        state.has_on = true;
        state.on = json["on"].toObject()["on"].toBool();
    }

    if (json.contains("dimming")) {
        state.has_dimming = true;
        state.brightness = json["dimming"].toObject()["brightness"].toDouble();
    }

    if (json.contains("color")) {
        state.has_color = true;
        x = json["color"].toObject()["xy"].toObject()["x"].toDouble();
        y = json["color"].toObject()["xy"].toObject()["y"].toDouble();
        state.color = XYBriToColor(x, y, 255);
    }

    if (json.contains("color_temperature")) {
        state.has_mirek = true;
        state.mirek_temperature = json["color_temperature"].toObject()["mirek"].toDouble();
        state.mirek_min = json["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_minimum"].toDouble();
        state.mirek_max = json["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_maximum"].toDouble();
        int kelvin = mirekToKelvin(state.mirek_temperature);
        QColor kelvin_color = kelvinToColor(kelvin);
        state.mirek_color = kelvin_color;
    }

    if (json.contains("gradient")) {
        state.has_gradient = true;
        state.gradient_points_capable = json["gradient"].toObject()["points_capable"].toDouble();

        QJsonArray gradient_points_array = json["gradient"].toObject()["points"].toArray();

        state.gradient_points.clear();

        if (gradient_points_array.size() == state.gradient_points_capable) {
            for (int i = 0; i < state.gradient_points_capable; ++i) {
                x = gradient_points_array[i].toObject()["color"].toObject()["xy"].toObject()["x"].toDouble();
                y = gradient_points_array[i].toObject()["color"].toObject()["xy"].toObject()["y"].toDouble();

                QColor gradient_point = XYBriToColor(x, y, 255);
                state.gradient_points.append(gradient_point);
            }
        }
    }

    if (json.contains("services")) {
        QJsonArray services = json["services"].toArray();
        for (int i = 0; i < services.size(); ++i) {
            QJsonObject json_service = services[i].toObject();

            state.services[json_service["rid"].toString()] = json_service["rtype"].toString();

            if (json_service["rtype"].toString() == "grouped_light") {
                state.grouped_light_rid = json_service["rid"].toString();
            }
        }
    }

    if (json.contains("children")) {
        QJsonArray children = json["children"].toArray();
        for (int i = 0; i < children.size(); ++i) {
            QJsonObject json_child = children[i].toObject();

            state.children[json_child["rid"].toString()] = json_child["rtype"].toString();
        }
    }

    if (json.contains("group")) {
        state.group_id = json["group"].toObject()["rid"].toString();
        state.group_type = json["group"].toObject()["rtype"].toString();
    }

    return state;
}

bool colorIsBlack(QColor color)
{
    if (color.red() == 255 && color.green() == 255 && color.blue() == 255) {
        return true;
    }

    return false;
}

QColor combineTwoColors(QColor base, QColor joiner)
{
    double red  = (base.red() + joiner.red()) / 2.0;
    double green  = (base.green() + joiner.green()) / 2.0;
    double blue  = (base.blue() + joiner.blue()) / 2.0;
    double alpha  = (base.alpha() + joiner.alpha()) / 2.0;

    return QColor::fromRgb(red, green, blue, alpha);
}


// my wish is this function would work forever and nobody will need to read it, sorry:-)
ItemState combineTwoStates(ItemState base, ItemState joiner)
{
    ItemState state;

    state.id = base.id;
    state.type = base.type;
    state.name = base.name;
    state.archetype = base.archetype;

    state.has_on = base.has_on || joiner.has_on;
    state.on = base.on || joiner.on;

    if (! state.has_on) {
        return state;
    }

    state.has_dimming = base.has_dimming || joiner.has_dimming;
    if ((base.on && joiner.on) && base.has_dimming && joiner.has_dimming && base.brightness != 0 && joiner.brightness != 0) {
        state.brightness = (base.brightness + joiner.brightness) / 2.0;
    } else if (base.on && base.has_dimming && base.brightness != 0) {
        state.brightness = base.brightness;
    } else if (joiner.on && joiner.has_dimming && joiner.brightness != 0) {
        state.brightness = joiner.brightness;
    }

    state.has_color = base.has_color || joiner.has_color;
    if ((base.on && joiner.on) && base.has_color && joiner.has_color && !colorIsBlack(base.color) && !colorIsBlack(joiner.color)) {

        state.color = combineTwoColors(base.color, joiner.color);

    } else if (base.on && base.has_color && !colorIsBlack(base.color)) {
        state.color = base.color;
    } else if (joiner.on && joiner.has_color && !colorIsBlack(joiner.color)) {
        state.color = joiner.color;
    }

    state.has_mirek = base.has_mirek || joiner.has_mirek;
    if ((base.on && joiner.on) && base.has_mirek && joiner.has_mirek) {
        if (base.mirek_temperature != 0 && joiner.mirek_temperature != 0) {
            state.mirek_temperature = (base.mirek_temperature + joiner.mirek_temperature) / 2.0;
        } else if (base.mirek_temperature != 0) {
            state.mirek_temperature = base.mirek_temperature;
        } else if (joiner.mirek_temperature != 0) {
            state.mirek_temperature = joiner.mirek_temperature;
        }

        state.mirek_min = qMin(base.mirek_min, joiner.mirek_min);
        state.mirek_max = qMax(base.mirek_max, joiner.mirek_max);
    } else if (base.on && base.has_mirek) {
        state.mirek_min = base.mirek_min;
        state.mirek_max = base.mirek_max;
    } else if (joiner.on && joiner.has_mirek) {
        state.mirek_min = joiner.mirek_min;
        state.mirek_max = joiner.mirek_max;
    }

    // we need to combine mirek color and ordinary color
    if (! base.has_color && ! joiner.has_color && joiner.has_color && !colorIsBlack(base.color) && !colorIsBlack(joiner.color)) {
        if (base.on && base.has_mirek && joiner.on && joiner.has_mirek) {
            state.color = combineTwoColors(base.mirek_color, joiner.mirek_color);
            state.has_color = true;
        } else if (base.on && base.has_mirek && (!joiner.has_mirek || !joiner.on) && !colorIsBlack(base.color)) {
            state.color = base.mirek_color;
            state.has_color = true;
        } else if ((!base.has_mirek || !base.on)&& joiner.on && joiner.has_mirek && !colorIsBlack(joiner.color)) {
            state.color = joiner.mirek_color;
            state.has_color = true;
        }
    }
    if (base.on && base.has_color && joiner.on && ! joiner.has_color && joiner.has_mirek && !colorIsBlack(base.color) && !colorIsBlack(joiner.mirek_color)) {
        state.color = combineTwoColors(base.color, joiner.mirek_color);
        state.has_color = true;
    }
    if (joiner.on && joiner.has_color && base.on && ! base.has_color && base.has_mirek && !colorIsBlack(base.mirek_color) && !colorIsBlack(joiner.color)) {
        state.color = combineTwoColors(joiner.color, base.mirek_color);
        state.has_color = true;
    }
    if (base.on && !joiner.on && base.has_mirek && ! base.has_color && !colorIsBlack(base.mirek_color)) {
        state.color = base.mirek_color;
        state.has_color = true;
    }
    if (joiner.on && !base.on && joiner.has_mirek && ! joiner.has_color && !colorIsBlack(joiner.mirek_color)) {
        state.color = joiner.mirek_color;
        state.has_color = true;
    }

    return state;
}

void delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QColor XYBriToColor(double x, double y, int bri)
{
    double z = 1.0 - x - y;
    double Y = bri / 255.0;
    double X = (Y / y) * x;
    double Z = (Y / y) * z;

    double r = X * 1.612 - Y * 0.203 - Z * 0.302;
    double g = -X * 0.509 + Y * 1.412 + Z * 0.066;
    double b = X * 0.026 - Y * 0.072 + Z * 0.962;

    r = r <= 0.0031308 ? 12.92 * r : (1.0 + 0.055) * qPow(r, (1.0 / 2.4)) - 0.055;
    g = g <= 0.0031308 ? 12.92 * g : (1.0 + 0.055) * qPow(g, (1.0 / 2.4)) - 0.055;
    b = b <= 0.0031308 ? 12.92 * b : (1.0 + 0.055) * qPow(b, (1.0 / 2.4)) - 0.055;

    double maxValue = qMax(r, qMax(g,b));

    r /= maxValue;
    g /= maxValue;
    b /= maxValue;

     /* do not know why thay have if (r < 0) { r = 255 }; this works better */
    r = r * 255; if (r < 0) { r *= -1; }
    g = g * 255; if (g < 0) { g *= -1; }
    b = b * 255; if (b < 0) { b *= -1; }

    if (r > 255) { r = 0; }
    if (g > 255) { g = 0; }
    if (b > 255) { b = 0; }

    r = qRound(r);
    g = qRound(g);
    b = qRound(b);

    return QColor::fromRgb(r, g, b, 255);
}

QVarLengthArray<float> colorToHueXY(QColor color)
{
    QVarLengthArray<float> normalizedToOne;
    float red;
    float green;
    float blue;

    normalizedToOne.append(color.red() / 255.0);
    normalizedToOne.append(color.green() / 255.0);
    normalizedToOne.append(color.blue() / 255.0);

    // Make red more vivid
    if (normalizedToOne[0] > 0.04045) {
        red = qPow((normalizedToOne[0] + 0.055) / (1.0 + 0.055), 2.4);
    } else {
        red = (normalizedToOne[0] / 12.92);
    }

    // Make green more vivid
    if (normalizedToOne[1] > 0.04045) {
        green = qPow((normalizedToOne[1] + 0.055) / (1.0 + 0.055), 2.4);
    } else {
        green = (normalizedToOne[1] / 12.92);
    }

    // Make blue more vivid
    if (normalizedToOne[2] > 0.04045) {
        blue = qPow((normalizedToOne[2] + 0.055) / (1.0 + 0.055), 2.4);
    } else {
        blue = (normalizedToOne[2] / 12.92);
    }

    float X = (red * 0.649926 + green * 0.103455 + blue * 0.197109);
    float Y = (red * 0.234327 + green * 0.743075 + blue * 0.022598);
    float Z = (red * 0.0000000 + green * 0.053077 + blue * 1.035763);

    float x = X / (X + Y + Z);
    float y = Y / (X + Y + Z);

    QVarLengthArray<float> ret;

    ret.append(x);
    ret.append(y);

    return ret;
}

QColor kelvinToColor(int kelvin)
{
    float tmpCalc = 0;
    float tmpKelvin = kelvin;
    float red = 0;
    float green = 0;
    float blue = 0;

    if (tmpKelvin < 1000) {
        tmpKelvin = 1000;
    }

    if (tmpKelvin > 40000) {
        tmpKelvin = 40000;
    }

    tmpKelvin = tmpKelvin / 100.0;

    if (tmpKelvin <= 66) {
        red = 255;
    } else {
        tmpCalc = tmpKelvin - 60;
        tmpCalc = 329.698727446 * qPow(tmpCalc, -0.1332047592);

        red = tmpCalc;
        if (red < 0) {red = 0;}
        if (red > 255) {red = 255;}
    }

    if (tmpKelvin <= 66) {
        tmpCalc = tmpKelvin;
        tmpCalc = 99.4708025861 * qLn(tmpCalc) - 161.1195681661;

        green = tmpCalc;
        if (green < 0) {green = 0;}
        if (green > 255) {green = 255;}
    } else {
        tmpCalc = tmpKelvin - 60;
        tmpCalc = 288.1221695283 * qPow(tmpCalc, -0.0755148492);

        green = tmpCalc;
        if (green < 0) {green = 0;}
        if (green > 255) {green = 255;}
    }

    if (tmpKelvin >= 66) {
        blue = 255;
    } else if (tmpKelvin <=19) {
        blue = 0;
    } else {
        tmpCalc = tmpKelvin - 10;
        tmpCalc = 138.5177312231 * qLn(tmpCalc) - 305.0447927307;

        blue = tmpCalc;
        if (blue < 0) {blue = 0;}
        if (blue > 255) {blue = 255;}
    }

    return QColor(red, green, blue);
}

int mirekToKelvin(int mirek)
{
    //the warmest color 2000K is 500 mirek
    //the coldest color 6500K is 153 mirek.

    if (mirek < 153)
        mirek = 153;

    if (mirek > 500)
        mirek = 500;

    int kelvin;
    kelvin = 6500.0 - ((4500.0 / 347.0) * (mirek - 153));
    return kelvin;
}