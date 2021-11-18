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

#ifndef HUEBRIDGE_H
#define HUEBRIDGE_H

#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QString>
#include <QJsonObject>

#include "huedevice.h"

enum HueBridgeRequestTypes {
    req_discovery_bridges,
    req_discovery_bridge,
    req_create_user,
    req_bridge_status_v1,
    req_bridge_config_v1,
    req_bridge_status_v2,
    req_bridge_put_v2
};

class HueBridgeDiscovery : public QObject
{
    Q_OBJECT
    public:
        explicit HueBridgeDiscovery(QObject *parent = nullptr);
        void discoverBridges();
        void discoverBridge(QString ip);

    private:
        const QString discover_url = "https://discovery.meethue.com/";
        QNetworkAccessManager *manager;

        void readBridges(QJsonArray &data);
        void readBridge(QJsonObject &data, QString ip);

    signals:
        void bridgeDiscovered(QJsonObject data, QString ip);

    private slots:
        void requestFinished(QNetworkReply *reply);
};




class HueBridge : public HueDevice
{
    Q_OBJECT
    public:
        explicit HueBridge(QString ip = "unknown", HueDevice *parent = nullptr);
        void setUserName(QString s);
        bool updateBridgeInfo(QJsonObject data);
        QJsonObject dumpBridge();
        void createUser();
        void getStatus1();
        void getConfig1();
        void getStatus();

        void putLight(QString id, QJsonObject json);
        void putGroupedLight(QString id, QJsonObject json);
        void putScene(QString id, QJsonObject json);

    private:
        QString url_api_v1 = "http://%1/api";
        QString url_api_v1_user = url_api_v1 + "/%2/%3";
        QString url_api_v2 = "https://%1/clip/v2/resource";
        QString url_api_v2_event_stream = "https://%1/eventstream/clip/v2";
        QString user_name = "";
        QString client_key = "";
        bool events_running = false;
        QNetworkAccessManager *event_manager;
        int event_retries = 0;

        void readCreateUser(QString ret);
        void runEventStream();

    signals:
        void events(QJsonArray &json_array);
        void userCreationFailed();
        void userCreationSucceed();
        void infoUpdated();
        void statusV2(QJsonObject json);

    private slots:
        void bridgeRequestFinished(const QVariant type, const QString ret);
        void startEventStream();
        void stopEventStream();
        void eventRequestFinished(QNetworkReply *reply);
};
#endif // HUEBRIDGE_H