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