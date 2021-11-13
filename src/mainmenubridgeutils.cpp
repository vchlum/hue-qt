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

#include "mainmenubridgeutils.h"

QJsonObject getItemById(QJsonObject json, QString id)
{
    QJsonObject ret;

    if (! json.contains("data")) {
        return ret;
    }

    QJsonArray json_array = json["data"].toArray();

    for (int i = 0; i < json_array.size(); ++i) {
        QJsonObject j = json_array[i].toObject();

        if (j.contains("id") && j["id"].toString() == id) {
            return j;
        }
    }

    return ret;
}

QString getTypeById(QJsonObject json, QString id)
{
    QString ret = "";

    QJsonObject item = getItemById(json, id);

    if (item.contains("type")) {
        return item["type"].toString();
    }

    return ret;
}

QJsonArray getItemsByType(QJsonObject json, QString type)
{
    QJsonArray ret;

    if (! json.contains("data")) {
        return ret;
    }

    QJsonArray json_array = json["data"].toArray();

    for (int i = 0; i < json_array.size(); ++i) {
        QJsonObject j = json_array[i].toObject();

        if (j.contains("type") && j["type"].toString() == type) {
            ret.append(j);
        }
    }

    return ret;
}

QJsonArray getServicesById(QJsonObject json, QString id)
{
    QJsonArray ret;

    QJsonObject item = getItemById(json, id);

    if (! item.contains("services")) {
        return ret;
    }

    return item["services"].toArray();
}

ItemState getLightState(QJsonObject json, QString id)
{
    ItemState state;
    QJsonObject item = getItemById(json, id);

    if (item.contains("on")) {
        state.has_on = true;
        state.on_all = item["on"].toObject()["on"].toBool();

        // we have only one light now
        state.on_any = state.on_all;
    }

    if (item.contains("dimming")) {
        state.has_dimming = true;
        state.brightness = item["dimming"].toObject()["brightness"].toDouble();
    }

    if (item.contains("color")) {
        state.has_color = true;
        double x = item["color"].toObject()["xy"].toObject()["x"].toDouble();
        double y = item["color"].toObject()["xy"].toObject()["y"].toDouble();
        state.color = XYBriToColor(x, y, 255);
    }

    if (item.contains("color_temperature")) {
        state.has_color_temperature = true;
        state.color_temperature = item["color_temperature"].toObject()["mirek"].toDouble();
        state.color_temperature_minimum = item["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_minimum"].toDouble();
        state.color_temperature_maximum = item["color_temperature"].toObject()["mirek_schema"].toObject()["mirek_maximum"].toDouble();
    }

    return state;
}

ItemState combineTwoStates(ItemState a, ItemState b)
{
    ItemState state;

    state.has_on = a.has_on || b.has_on;

    if (a.has_on && b.has_on) {
        state.on_all = a.on_all && b.on_all;
        state.on_any = a.on_any || b.on_any;
    } else if (a.has_on){
        state.on_all = a.on_all;
        state.on_any = a.on_any;
    } else if (b.has_on){
        state.on_all = b.on_all;
        state.on_any = b.on_any;
    }

    if (! state.has_on) {
        return state;
    }

    state.has_dimming = a.has_dimming || b.has_dimming;
    if ((a.on_any && b.on_any) && a.has_dimming && b.has_dimming) {
        state.brightness = (a.brightness + b.brightness) / 2.0;
    } else if (a.on_any && a.has_dimming) {
        state.brightness = a.brightness;
    } else if (b.on_any && b.has_dimming) {
        state.brightness = b.brightness;
    }

    state.has_color = a.has_color || b.has_color;
    if ((a.on_any && b.on_any) && a.has_color && b.has_color) {
        double red  = (a.color.red() + b.color.red()) / 2.0;
        double green  = (a.color.green() + b.color.green()) / 2.0;
        double blue  = (a.color.blue() + b.color.blue()) / 2.0;
        double alpha  = (a.color.alpha() + b.color.alpha()) / 2.0;

        state.color = QColor::fromRgb(red, green, blue, alpha);

    } else if (a.on_any && a.has_color) {
        state.color = a.color;
    } else if (b.on_any && b.has_color) {
        state.color = b.color;
    }

    state.has_color_temperature = a.has_color_temperature || b.has_color_temperature;
    if ((a.on_any && b.on_any) && a.has_color_temperature && b.has_color_temperature) {
        if (a.color_temperature != 0 && b.color_temperature != 0) {

            state.color_temperature = (a.color_temperature + b.color_temperature) / 2;

        } else if (a.color_temperature != 0) {
            state.color_temperature = a.color_temperature;
        } else if (b.color_temperature != 0) {
            state.color_temperature = b.color_temperature;
        }

        state.color_temperature_minimum = qMin(a.color_temperature_minimum, b.color_temperature_minimum);
        state.color_temperature_maximum = qMax(a.color_temperature_maximum, b.color_temperature_maximum);
    } else if (a.on_any && a.has_color_temperature) {
        state.color_temperature_minimum = a.color_temperature_minimum;
        state.color_temperature_maximum = a.color_temperature_maximum;
    } else if (b.on_any && b.has_color_temperature) {
        state.color_temperature_minimum = b.color_temperature_minimum;
        state.color_temperature_maximum = b.color_temperature_maximum;
    }

    return state;
}

ItemState getStateById(QJsonObject json, QString id)
{
    ItemState state;
    QString type = getTypeById(json, id);

    if (type == "room" || type == "zone" || type == "bridge_home") {
        QJsonArray services_array = getServicesById(json, id);

        state.ids.append(id);

        for (int i = 0; i < services_array.size(); ++i) {
            QJsonObject item = services_array[i].toObject();

            QString rid = item["rid"].toString();
            ItemState partial_state = getStateById(json, rid);

            state = combineTwoStates(state, partial_state);

            state.ids.append(rid);
        }
    }

    if (type == "light") {
        state = getLightState(json, id);

        state.ids.append(id);
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