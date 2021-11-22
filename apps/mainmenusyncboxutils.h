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

#ifndef MAINMENUSYNCBOXUTILS_H
#define MAINMENUSYNCBOXUTILS_H

#include <QJsonObject>
#include <QJsonArray>

const QVarLengthArray<QString> inputs {
    "input1", "input2", "input3", "input4"
};

const QMap<QString, QString> mode_list {
    {"game", QObject::tr("Game")},
    {"video", QObject::tr("Video")},
    {"music", QObject::tr("Music")}
};


const QVarLengthArray<QString> intensity_ordered {
    "subtle", "moderate", "high", "intense"
};

const QMap<QString, QString> intensity_list {
    {"subtle", QObject::tr("Subtle")},
    {"moderate", QObject::tr("Moderate")},
    {"high", QObject::tr("High")},
    {"intense", QObject::tr("Intense")}
};

const QMap<QString, QString> mode_icon {
    {"game", ":images/HueIcons/roomsMancave.svg"},
    {"video", ":images/HueIcons/otherWatchingMovie.svg"},
    {"music", ":images/HueIcons/otherMusic.svg"}
};

struct SyncboxState {
    QString id = "";
    QString type = "";
    QString name = "";
    QString wifi_ssid = "";

    QString mode = "";
    bool sync_active = false;
    QString hdmi_source = "";
    QString last_sync_mode = "";
    int brightness;

    QString groupid = "";
    QMap<QString, QString> groups_list; //groupid, name

    QMap<QString, QString> mode_list; // mode, intensity

    QMap<QString, QString> hdmies_name_list; //input, name
    QMap<QString, QString> hdmies_type_list; //input, type
    QMap<QString, QString> hdmies_status_list; //input, status
    QMap<QString, QString> hdmies_sync_mode_list; //input, sync_mode
};

#endif // MAINMENUSYNCBOXUTILS_H