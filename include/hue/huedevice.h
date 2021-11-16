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

#ifndef HUEDEVICE_H
#define HUEDEVICE_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>

#define HUEREQUEST_TYPE (QNetworkRequest::Attribute) (((int) QNetworkRequest::User) + 0)
#define HUEREQUEST_IP (QNetworkRequest::Attribute) (((int) QNetworkRequest::User) + 1)

class HueDevice : public QObject
{
    Q_OBJECT
    public:
        QList<QStringList> request_headers;

        explicit HueDevice(QString address = "unknown", QObject *parent = nullptr);

        void setDeviceName(QString s);
        QString deviceName();

        void setIp(QString s);
        QString ip();

        void setKnown();
        bool known();

        void setId(QString s);
        QString id();

        bool deviceConnected();

        QStringList createHeader(QString key, QString value);
        void sendRequestGET(QString url, QNetworkRequest::Attribute type);
        void sendRequestPUT(QString url, QNetworkRequest::Attribute type, const QByteArray json_data);
        void sendRequestPOST(QString url, QNetworkRequest::Attribute type, const QByteArray data);
        void sendRequestDELETE(QString url, QNetworkRequest::Attribute type);

    private:
        QNetworkAccessManager *manager;
        QString ip_address;
        QString identifier = "";
        QString device_name = "";
        bool paired = false;
        bool device_connected = false;

    signals:
        void requestDeviceFinished(const QVariant type, const QString ret);
        void connected();
        void disconnected();

    private slots:
        void onSslError(QNetworkReply* r, QList<QSslError> l);
        void requestFinished(QNetworkReply *reply);
};
#endif // HUEDEVICE_H