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

#ifndef HUESYNCBOX_H
#define HUESYNCBOX_H

#include <QJsonObject>
#include <QTimer>

#include "huedevice.h"

enum HueSyncboxRequestTypes {
    req_registration,
    req_device,
    req_syncbox_status
};

class HueSyncbox : public HueDevice
{
    Q_OBJECT
    public:
        explicit HueSyncbox(QString ip = "unknown", HueDevice *parent = nullptr);
        void setAccessToken(QString s);
        bool updateSyncboxInfo(QJsonObject data);
        QJsonObject dumpSyncbox();
        void createRegistration();
        void getDevice();
        void getStatus();

    private:
        QString url_api_v1 = "https://%1/api/v1/%2";;
        int registration_counter;
        QTimer *registration_timer;
        QString access_token = "";
        QString registration_id;

        void readRegistration(QString ret);

    signals:
        void registrationFailed();
        void registrationSucceed();
        void infoUpdated();

    private slots:
        void syncboxRequestFinished(const QVariant type, const QString ret);
        void tryRegister();
};
#endif // HUESYNCBOX_H