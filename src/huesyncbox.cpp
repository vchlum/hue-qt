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
#include "huesyncbox.h"

HueSyncbox::HueSyncbox(QString ip, HueDevice *parent): HueDevice(ip, parent)
{
    connect(this, SIGNAL(requestDeviceFinished(const QVariant, const QString)), this, SLOT(syncboxRequestFinished(const QVariant, const QString)));
}

void HueSyncbox::setAccessToken(QString s)
{
    access_token = s;

    request_headers.clear();
    request_headers.append(createHeader("ssl", "False"));
    request_headers.append(createHeader("Authorization", "Bearer " + access_token));

    setKnown();
}

bool HueSyncbox::updateSyncboxInfo(QJsonObject data)
{
    bool changed = false;

    if (data.contains("uniqueId")) {
        if (id() != data["uniqueId"].toString()) {
            setId(data["uniqueId"].toString());
            changed = true;
        }
    }

    if (data.contains("name")) {
        if (deviceName() != data["name"].toString()) {
            setDeviceName(data["name"].toString());
            changed = true;
        }
    }

    if (data.contains("ipAddress")) {
        if (ip() != data["ipAddress"].toString()) {
            setIp(data["ipAddress"].toString());
            changed = true;
        }
    }

    if (data.contains("accessToken")) {
        if (access_token != data["accessToken"].toString()) {
            setAccessToken(data["accessToken"].toString());
            changed = true;
        }
    }

    if (data.contains("registrationId")) {
        if (registration_id != data["registrationId"].toString()) {
            registration_id = data["registrationId"].toString();
            changed = true;
        }
    }

    return changed;
}

QJsonObject HueSyncbox::dumpSyncbox()
{
    QJsonObject json;

    json["type"] = "syncbox";
    json["uniqueId"] = id();
    json["name"] = deviceName();
    json["ipAddress"] = ip();
    json["accessToken"] = access_token;
    json["registrationId"] = registration_id;

    return json;
}

void HueSyncbox::createRegistration()
{
    registration_counter = 0;

    registration_timer = new QTimer(this);

    tryRegister();
}

void HueSyncbox::tryRegister()
{
    registration_counter++;

    if (known()) {
        return;
    }

    if (registration_counter > 8) {
        emit registrationFailed();
        return;
    }

    QString host_name = QHostInfo::localHostName().left(10);

    QJsonObject json;
    json["appName"] = "hue-qt";
    json["instanceName"] = host_name;

    QString url = url_api_v1.arg(ip(), "registrations");

    QJsonDocument doc(json);
    QByteArray bytes = doc.toJson();

    sendRequestPOST(url, (QNetworkRequest::Attribute) req_registration, bytes);

    registration_timer->singleShot(3000, this, SLOT(tryRegister()));
}

void HueSyncbox::syncboxRequestFinished(const QVariant type, const QString ret)
{
    HueSyncboxRequestTypes hue_type = (HueSyncboxRequestTypes) type.toInt();

    switch (hue_type) {
        case req_registration:
            {
                readRegistration(ret);
                break;
            }

        case req_device:
            {
                QString device_status = ret;
                QJsonObject json = QString2QJsonObject(device_status);
                if (updateSyncboxInfo(json)) {
                    emit infoUpdated();
                }
                break;
            }

        case req_syncbox_status:
            {
                QString device_status = ret;
                QJsonObject json = QString2QJsonObject(device_status);
                emit status(json);
                break;
            }

        case req_syncbox_put_execution:
            {
                emit executionFinished();
            }
        default:
            {
                break;
            }
    }
}

void HueSyncbox::readRegistration(QString ret)
{
    QJsonObject json = QString2QJsonObject(ret);
    if (updateSyncboxInfo(json)) {
        emit infoUpdated();
    }

    if (access_token != "") {
        emit registrationSucceed();
        getDevice();
    }
}

void HueSyncbox::getDevice()
{
    QString url = url_api_v1.arg(ip(), "device");

    sendRequestGET(url, (QNetworkRequest::Attribute) req_device);
}

void HueSyncbox::getStatus()
{
    QString url = url_api_v1.arg(ip(), "");

    sendRequestGET(url, (QNetworkRequest::Attribute) req_syncbox_status);
}

void HueSyncbox::setExecution(QJsonObject json)
{
    QString url = url_api_v1.arg(ip(), "execution");
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    sendRequestPUT(url, (QNetworkRequest::Attribute) req_syncbox_put_execution, data);
}

void HueSyncbox::setPower(bool on)
{
    QJsonObject json;
    if (on) {
        setMode("passthrough");
    } else {
        setMode("powersave");
    }
}

void HueSyncbox::setSync(bool on)
{
    QJsonObject json;
    json["syncActive"] = on;
    setExecution(json);
}

void HueSyncbox::setMode(QString mode)
{
    QJsonObject json;
    json["mode"] = mode;
    setExecution(json);
}

void HueSyncbox::setIntensity(QString intensity)
{
    QJsonObject json;
    json["intensity"] = intensity;
    setExecution(json);
}

void HueSyncbox::setBrightness(int brightness)
{
    QJsonObject json;
    json["brightness"] = brightness;
    setExecution(json);
}

void HueSyncbox::setInput(QString input)
{
    QJsonObject json;
    json["hdmiSource"] = input;
    setExecution(json);
}

void HueSyncbox::setGroup(QString groupid)
{
    QJsonObject json;
    json["hueTarget"] = groupid;
    setExecution(json);
}