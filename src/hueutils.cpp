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

#include <QDebug>
#include <QJsonDocument>

#include "hueutils.h"

QJsonArray QString2QJsonArray(QString &data)
{
    QJsonArray arr;

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

    if (doc.isNull()) {
        return arr;
    }

    if (!doc.isArray()) {
        return arr;
    }

    arr = doc.array();

    return arr;
}

QJsonObject QString2QJsonObject(QString &data)
{
    QJsonObject obj;

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

    if (doc.isNull()) {
        return obj;
    }

    if (! doc.isObject()) {
        return obj;
    }

    obj = doc.object();

    return obj;
}