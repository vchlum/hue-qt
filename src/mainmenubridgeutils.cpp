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
        double x = json["color"].toObject()["xy"].toObject()["x"].toDouble();
        double y = json["color"].toObject()["xy"].toObject()["y"].toDouble();
        state.color = XYBriToColor(x, y, 255);
    }

    if (json.contains("color_temperature")) {
        state.has_color_temperature = true;
        state.color_temperature = json["color_temperature"].toObject()["mirek"].toDouble();
        state.color_temperature_minimum = json["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_minimum"].toDouble();
        state.color_temperature_maximum = json["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_maximum"].toDouble();
    }

    if (json.contains("gradient")) {
        state.has_gradient = true;
        state.gradient_points_capable = json["gradient"].toObject()["points_capable"].toDouble();
    }

    return state;
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

    if (json.contains("services")) {
        QJsonArray services = json["services"].toArray();
        for (int i = 0; i < services.size(); ++i) {
            QJsonObject json_service = services[i].toObject();

            state.services[json_service["rid"].toString()] = json_service["rtype"].toString();
        }
    }

    return state;
}

ItemState getSceneFromJson(QJsonObject json)
{
    ItemState state;
    state.id = json["id"].toString();
    state.type = json["type"].toString();

    if (json.contains("metadata")) {
        state.name = json["metadata"].toObject()["name"].toString();
    }

    if (json.contains("group")) {
        state.group_id = json["group"].toObject()["rid"].toString();
        state.group_type = json["group"].toObject()["rtype"].toString();
    }

    return state;
}

ItemState updateState(ItemState state, QJsonObject json)
{
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
        double x = json["color"].toObject()["xy"].toObject()["x"].toDouble();
        double y = json["color"].toObject()["xy"].toObject()["y"].toDouble();
        state.color = XYBriToColor(x, y, 255);
    }

    if (json.contains("color_temperature")) {
        state.has_color_temperature = true;
        state.color_temperature = json["color_temperature"].toObject()["mirek"].toDouble();
        state.color_temperature_minimum = json["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_minimum"].toDouble();
        state.color_temperature_maximum = json["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_maximum"].toDouble();
    }

    if (json.contains("gradient")) {
        state.has_gradient = true;
        state.gradient_points_capable = json["gradient"].toObject()["points_capable"].toDouble();
    }

    return state;
}

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
    if ((base.on && joiner.on) && base.has_dimming && joiner.has_dimming) {
        state.brightness = (base.brightness + joiner.brightness) / 2.0;
    } else if (base.on && base.has_dimming) {
        state.brightness = base.brightness;
    } else if (joiner.on && joiner.has_dimming) {
        state.brightness = joiner.brightness;
    }

    state.has_color = base.has_color || joiner.has_color;
    if ((base.on && joiner.on) && base.has_color && joiner.has_color) {
        double red  = (base.color.red() + joiner.color.red()) / 2.0;
        double green  = (base.color.green() + joiner.color.green()) / 2.0;
        double blue  = (base.color.blue() + joiner.color.blue()) / 2.0;
        double alpha  = (base.color.alpha() + joiner.color.alpha()) / 2.0;

        state.color = QColor::fromRgb(red, green, blue, alpha);

    } else if (base.on && base.has_color) {
        state.color = base.color;
    } else if (joiner.on && joiner.has_color) {
        state.color = joiner.color;
    }

    state.has_color_temperature = base.has_color_temperature || joiner.has_color_temperature;
    if ((base.on && joiner.on) && base.has_color_temperature && joiner.has_color_temperature) {
        if (base.color_temperature != 0 && joiner.color_temperature != 0) {
            state.color_temperature = (base.color_temperature + joiner.color_temperature) / 2.0;
        } else if (base.color_temperature != 0) {
            state.color_temperature = base.color_temperature;
        } else if (joiner.color_temperature != 0) {
            state.color_temperature = joiner.color_temperature;
        }

        state.color_temperature_minimum = qMin(base.color_temperature_minimum, joiner.color_temperature_minimum);
        state.color_temperature_maximum = qMax(base.color_temperature_maximum, joiner.color_temperature_maximum);
    } else if (base.on && base.has_color_temperature) {
        state.color_temperature_minimum = base.color_temperature_minimum;
        state.color_temperature_maximum = base.color_temperature_maximum;
    } else if (joiner.on && joiner.has_color_temperature) {
        state.color_temperature_minimum = joiner.color_temperature_minimum;
        state.color_temperature_maximum = joiner.color_temperature_maximum;
    }

    return state;
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