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

#include <QFile>
#include <QJsonArray>

#include "huebridgelist.h"

HueBridgeList::HueBridgeList(HueList *parent): HueList(parent)
{

}

HueBridge* HueBridgeList::findBridge(QString s)
{
    if (s == "") {
        return NULL;
    }

    foreach(HueBridge *bridge, list) {
        if (bridge->id() == s) {
            return bridge;
        }

        if (bridge->ip() == s) {
            return bridge;
        }
    }

    return NULL;
}

HueBridge* HueBridgeList::addBridge(QString ip)
{
    HueBridge *bridge;
    bridge = findBridge(ip);
    if (bridge != NULL) {
        return bridge;
    }

    bridge = new HueBridge(ip);

    connect(bridge, SIGNAL(userCreationSucceed()), this, SLOT(needSave()));
    connect(bridge, SIGNAL(infoUpdated()), this, SLOT(needSave()));

    list.append(bridge);

    return bridge;
}

void HueBridgeList::needSave()
{
    saveBridges();
    emit bridgeDataUpdated();
}

void HueBridgeList::checkBridges()
{
    foreach(HueBridge *bridge, list) {

    }
}

void HueBridgeList::createBridge(QJsonObject data, QString ip)
{
    HueBridge *bridge = NULL;

    /* check if bridgeid exists, if yes - update info */
    if (data.contains("bridgeid") &&
        data["bridgeid"].isString()) {

        QString id = data["bridgeid"].toString();
        bridge = findBridge(id);
    };

    if (bridge == NULL) {
        bridge = addBridge(ip);
    }

    QJsonObject data_ip;
    data_ip["internalipaddress"] = ip;

    bridge->updateBridgeInfo(data_ip);
    bridge->updateBridgeInfo(data);
}

void HueBridgeList::saveBridges()
{
    QFile f(QStringLiteral("bridge.json"));
    QJsonArray json_array;

    foreach(HueBridge *bridge, list) {
        if (!bridge->known()) {
            continue;
        }

        json_array.append(bridge->dumpBridge());
    }

    QJsonDocument doc(json_array);

    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open bridge save file.";
        return;
    }

    f.write(doc.toJson());
}

void HueBridgeList::loadBridges()
{
    QFile f(QStringLiteral("bridge.json"));
    QJsonArray json_array;

    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open bridge save file.";
        return;
    }

    QByteArray data = f.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    json_array = doc.array();

    for (int i = 0; i < json_array.size(); ++i) {
        QJsonObject bridge = json_array[i].toObject();

        if (bridge.contains("internalipaddress") &&
            bridge["internalipaddress"].isString()) {

            QString ip = bridge["internalipaddress"].toString();

            createBridge(bridge, ip);
        }
    }
}
