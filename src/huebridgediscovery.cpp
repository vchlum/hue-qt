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

#include <QDebug>
#include <QJsonArray>
#include <QVariant>

#include "hueutils.h"
#include "huebridge.h"

HueBridgeDiscovery::HueBridgeDiscovery(QObject *parent): QObject(parent)
{
    manager = new QNetworkAccessManager();
    
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

void HueBridgeDiscovery::requestFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qWarning() << "request reply - failed to discover bridge(s)";
        return;
    }

    QString ret = reply->readAll();

    QNetworkRequest request = reply->request();

    HueBridgeRequestTypes hue_type = (HueBridgeRequestTypes) request.attribute(HUEREQUEST_TYPE).toInt();

    switch (hue_type) {
        case req_discovery_bridges:
            {
                QJsonArray array_data = QString2QJsonArray(ret);
                readBridges(array_data);
                break;
            }

        case req_discovery_bridge:
            {
                QJsonObject object_data = QString2QJsonObject(ret);
                QString ip = request.attribute(HUEREQUEST_IP).toString();
                readBridge(object_data, ip);
                break;
            }

        default:
            {
                break;
            }
    }
}

void HueBridgeDiscovery::discoverBridges()
{
    QNetworkRequest request;
    request.setUrl(QUrl(discover_url));
    request.setAttribute(HUEREQUEST_TYPE, req_discovery_bridges);
    manager->get(request);
}

void HueBridgeDiscovery::discoverBridge(QString ip)
{
    QNetworkRequest request;
    QString url = QString("http://%1/api/config").arg(ip);

    request.setUrl(QUrl(url));
    request.setAttribute(HUEREQUEST_TYPE, req_discovery_bridge);
    request.setAttribute(HUEREQUEST_IP, ip);
    manager->get(request);
}

void HueBridgeDiscovery::readBridges(QJsonArray &data)
{
    /* It is a bridge */
    if (data.size() == 0) {
        return;
    }

    for (int i = 0; i < data.size(); ++i) {
        QJsonObject bridge = data[i].toObject();
        if (bridge.contains("internalipaddress") &&
            bridge["internalipaddress"].isString()) {

            QString ip = bridge["internalipaddress"].toString();

            discoverBridge(ip);
        }
    }
}

void HueBridgeDiscovery::readBridge(QJsonObject &data, QString ip)
{
    /* It is the bridge discovery */
    if (data.size() == 0) {
        return;
    }

    if (data.contains("name") &&
        data["name"].isString()) {

        emit bridgeDiscovered(data, ip);
    }
}
