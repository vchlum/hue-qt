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

#include "huedevice.h"

using namespace std;

HueDevice::HueDevice(QString address, QObject *parent): QObject(parent)
{
    setIp(address);
    manager = new QNetworkAccessManager();
    request_headers.clear();

    connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(onSslError(QNetworkReply*, QList<QSslError>)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

void HueDevice::onSslError(QNetworkReply* r, QList<QSslError> l) {
    (void) l;

    if (!use_ssl) {
        r->ignoreSslErrors();
    }
}

void HueDevice::setDeviceName(QString s)
{
    device_name = s;
}

QString HueDevice::deviceName()
{
    return device_name;
}

void HueDevice::setIp(QString s)
{
    ip_address = s;
}

QString HueDevice::ip()
{
    return ip_address;
}

void HueDevice::setKnown()
{
    paired = true;
}

bool HueDevice::known()
{
    return paired;
}

void HueDevice::setId(QString s)
{
    identifier = s;
}


QString HueDevice::id()
{
    return identifier;
}

bool HueDevice::deviceConnected()
{
    return device_connected;
}

void HueDevice::setSslConfiguration(QSslConfiguration ssl_configuration)
{
    use_ssl = true;
    ssl_conf = ssl_configuration;
}

QStringList HueDevice::createHeader(QString key, QString value)
{
    QStringList header;

    header.append(key);
    header.append(value);

    return header;
}

void HueDevice::sendRequestGET(QString url, QNetworkRequest::Attribute type)
{
    QNetworkRequest request;

    if (use_ssl) {
        request.setSslConfiguration(ssl_conf);
        request.setPeerVerifyName(id());
    }

    request.setUrl(QUrl(url));
    request.setAttribute(HUEREQUEST_TYPE, type);
    foreach (const QStringList &header, request_headers) {
        if (header.isEmpty()) {
            break;
        }

        request.setRawHeader(header.at(0).toUtf8(), header.at(1).toUtf8());
    }

    manager->get(request);
}

void HueDevice::sendRequestPUT(QString url, QNetworkRequest::Attribute type, const QByteArray data)
{
    QNetworkRequest request;

    if (use_ssl) {
        request.setSslConfiguration(ssl_conf);
        request.setPeerVerifyName(id());
    }

    request.setUrl(QUrl(url));
    request.setAttribute(HUEREQUEST_TYPE, type);

    foreach (const QStringList &header, request_headers) {
        if (header.isEmpty()) {
            break;
        }

        request.setRawHeader(header.at(0).toUtf8(), header.at(1).toUtf8());
    }

    manager->put(request, data);
}

void HueDevice::sendRequestPOST(QString url, QNetworkRequest::Attribute type, const QByteArray data)
{
    QNetworkRequest request;

    if (use_ssl) {
        request.setSslConfiguration(ssl_conf);
        request.setPeerVerifyName(id());
    }

    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(HUEREQUEST_TYPE, type);

    foreach (const QStringList &header, request_headers) {
        if (header.isEmpty()) {
            break;
        }

        request.setRawHeader(header.at(0).toUtf8(), header.at(1).toUtf8());
    }

    manager->post(request, data);
}

void HueDevice::sendRequestDELETE(QString url, QNetworkRequest::Attribute type)
{
    QNetworkRequest request;

    if (use_ssl) {
        request.setSslConfiguration(ssl_conf);
        request.setPeerVerifyName(id());
    }

    request.setUrl(QUrl(url));
    request.setAttribute(HUEREQUEST_TYPE, type);
    foreach (const QStringList &header, request_headers) {
        if (header.isEmpty()) {
            break;
        }
        request.setRawHeader(header.at(0).toUtf8(), header.at(1).toUtf8());
    }

    manager->deleteResource(request);
}

void HueDevice::requestFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qWarning() << "request reply error: " + reply->errorString();

        if (known() && device_connected) {
            emit disconnected();
        }

        device_connected = false;
        return;
    }

    if (known() && !device_connected) {
        device_connected = true;

        emit connected();
    }

    const QString ret = reply->readAll();

    QNetworkRequest request = reply->request();

    const QVariant type = request.attribute(HUEREQUEST_TYPE);

    emit requestDeviceFinished(type, ret);
}
