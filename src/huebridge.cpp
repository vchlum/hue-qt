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
#include <QJsonObject>
#include <QVariant>
#include <QHostInfo>

#include "hueutils.h"
#include "huebridge.h"

HueBridge::HueBridge(QString ip, HueDevice *parent): HueDevice(ip, parent)
{
    event_manager = new QNetworkAccessManager();
    connect(event_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(onSslError(QNetworkReply*, QList<QSslError>)));
    connect(event_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(eventRequestFinished(QNetworkReply*)));

    connect(this, SIGNAL(requestDeviceFinished(const QVariant, const QString)), this, SLOT(bridgeRequestFinished(const QVariant, const QString)));

    connect(this, SIGNAL(connected()), this, SLOT(startEventStream()));
    connect(this, SIGNAL(disconnected()), this, SLOT(stopEventStream()));
}

void HueBridge::setUserName(QString s)
{
    user_name = s;

    request_headers.clear();
    request_headers.append(createHeader("ssl", "False"));
    request_headers.append(createHeader("hue-application-key", user_name));

    setKnown();
}

bool HueBridge::updateBridgeInfo(QJsonObject data)
{
    bool changed = false;

    if (data.contains("bridgeid")) {
        if (id() != data["bridgeid"].toString()) {
            setId(data["bridgeid"].toString());
            changed = true;
        }
    }

    if (data.contains("name")) {
        if (deviceName() != data["name"].toString()) {
            setDeviceName(data["name"].toString());
            changed = true;
        }
    }

    if (data.contains("internalipaddress")) {
        if (ip() != data["internalipaddress"].toString()) {
            setIp(data["internalipaddress"].toString());
            changed = true;
        }
    }

    if (data.contains("username")) {
        if (user_name != data["username"].toString()) {
            setUserName(data["username"].toString());
            changed = true;
        }
    }

    if (data.contains("clientkey")) {
        if (client_key != data["clientkey"].toString()) {
            client_key = data["clientkey"].toString();
            changed = true;
        }
    }

    return changed;
}

QJsonObject HueBridge::dumpBridge()
{
    QJsonObject json;

    json["type"] = "bridge";
    json["bridgeid"] = id();
    json["name"] = deviceName();
    json["internalipaddress"] = ip();
    json["username"] = user_name;
    json["clientkey"] = client_key;

    return json;
}

void HueBridge::createUser()
{
    QString user = "hue-qt#";
    user += QHostInfo::localHostName().left(15);

    QJsonObject json;
    json["devicetype"] = user;
    json["generateclientkey"] = true;

    QJsonDocument doc(json);
    QByteArray bytes = doc.toJson();

    QString url = url_api_v1.arg(ip());

    sendRequestPOST(url, (QNetworkRequest::Attribute) req_create_user, bytes);
}

void HueBridge::runEventStream()
{
    if (!events_running) {
        return;
    }

    QString url = url_api_v2_event_stream.arg(ip());

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork); // Events shouldn't be cached

    foreach (const QStringList &header, request_headers) {
        if (header.isEmpty()) {
            break;
        }

        request.setRawHeader(header.at(0).toUtf8(), header.at(1).toUtf8());
    }

    event_manager->get(request);
}


void HueBridge::startEventStream()
{
    events_running = true;
    event_retries = 0;
    runEventStream();
}

void HueBridge::stopEventStream()
{
    events_running = false;
}

void HueBridge::eventRequestFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        if(event_retries < 10) {
            qWarning() << "request reply on event stream error: " + reply->errorString();
            event_retries++;
            runEventStream();
            return;
        } else {
            qCritical() << "Unable to reconnect event stream, max retries reached";
            return;
        }
    }

    QString ret = reply->readAll();

    QJsonArray json_array = QString2QJsonArray(ret);

    if (json_array.size() > 0) {
        emit events(json_array);
    }

    event_retries = 0;
    runEventStream();
}

void HueBridge::bridgeRequestFinished(const QVariant type, const QString ret)
{
    HueBridgeRequestTypes hue_type = (HueBridgeRequestTypes) type.toInt();

    switch (hue_type) {
        case req_create_user:
            {
                readCreateUser(ret);
                break;
            }

        case req_bridge_status_v1:
            {
                break;
            }

        case req_bridge_status_v2:
            {
                QString device_status = ret;
                QJsonObject json = QString2QJsonObject(device_status);
                emit statusV2(json);
                break;
            }

        case req_bridge_config_v1:
            {
                QString device_config = ret;
                QJsonObject json = QString2QJsonObject(device_config);
                if (updateBridgeInfo(json)) {
                    emit infoUpdated();
                }
                break;
            }

        default:
            {
                break;
            }
    }
}

void HueBridge::readCreateUser(QString ret)
{
    QJsonArray json_array = QString2QJsonArray(ret);

    if (json_array.size() == 0) {
        return;
    }

    for (int i = 0; i < json_array.size(); ++i) {
        QJsonObject msg = json_array[i].toObject();
        if (msg.contains("success")) {
            QJsonObject json = msg["success"].toObject();
            if (updateBridgeInfo(json)) {
                emit infoUpdated();
            }

            getStatus();

            emit userCreationSucceed();
        }

        if (msg.contains("error")) {
            qWarning() << msg["error"].toObject()["description"].toString();
            emit userCreationFailed();
        }
    }
}

void HueBridge::getStatus1()
{
    QString url = url_api_v1_user.arg(ip(), user_name, "");
    sendRequestGET(url, (QNetworkRequest::Attribute) req_bridge_status_v1);
}

void HueBridge::getConfig1()
{
    QString url = url_api_v1_user.arg(ip(), user_name, "config");
    sendRequestGET(url, (QNetworkRequest::Attribute) req_bridge_config_v1);
}

void HueBridge::getStatus()
{
    QString url = url_api_v2.arg(ip());
    sendRequestGET(url, (QNetworkRequest::Attribute) req_bridge_status_v2);
}

void HueBridge::putLight(QString id, QJsonObject json)
{
    QString url = url_api_v2.arg(ip()) + "/light/" + id;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    sendRequestPUT(url, (QNetworkRequest::Attribute) req_bridge_put_v2 , data);
}

void HueBridge::putGroupedLight(QString id, QJsonObject json)
{
    QString url = url_api_v2.arg(ip()) + "/grouped_light/" + id;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    sendRequestPUT(url, (QNetworkRequest::Attribute) req_bridge_put_v2 , data);
}

void HueBridge::putScene(QString id, QJsonObject json)
{
    QString url = url_api_v2.arg(ip()) + "/scene/" + id;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    sendRequestPUT(url, (QNetworkRequest::Attribute) req_bridge_put_v2 , data);
}