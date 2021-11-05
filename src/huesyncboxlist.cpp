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

#include "huesyncboxlist.h"

HueSyncboxList::HueSyncboxList(HueList *parent): HueList(parent)
{

}

HueSyncbox* HueSyncboxList::findSyncbox(QString s)
{
    if (s == "") {
        return NULL;
    }

    foreach(HueSyncbox *syncbox, list) {
        if (syncbox->id() == s) {
            return syncbox;
        }

        if (syncbox->ip() == s) {
            return syncbox;
        }
    }

    return NULL;
}

HueSyncbox* HueSyncboxList::addSyncbox(QString ip)
{
    HueSyncbox *syncbox;
    syncbox = findSyncbox(ip);
    if (syncbox != NULL) {
        return syncbox;
    }

    syncbox = new HueSyncbox(ip);

    connect(syncbox, SIGNAL(registrationSucceed()), this, SLOT(needSave()));
    connect(syncbox, SIGNAL(infoUpdated()), this, SLOT(needSave()));

    list.append(syncbox);

    return syncbox;
}

void HueSyncboxList::needSave()
{
    saveSyncboxes();
    emit syncboxDataUpdated();
}

void HueSyncboxList::createSyncbox(QJsonObject data, QString ip)
{
    HueSyncbox *syncbox = NULL;

    /* check if bridgeid exists, if yes - update info */
    if (data.contains("uniqueId") &&
        data["uniqueId"].isString()) {

        QString id = data["uniqueId"].toString();
        syncbox = findSyncbox(id);
    };

    if (syncbox == NULL) {
        syncbox = addSyncbox(ip);
    }

    QJsonObject data_ip;
    data_ip["ipAddress"] = ip;

    syncbox->updateSyncboxInfo(data_ip);
    syncbox->updateSyncboxInfo(data);
}

void HueSyncboxList::saveSyncboxes()
{
    QFile f(QStringLiteral("syncbox.json"));
    QJsonArray json_array;

    foreach(HueSyncbox *bridge, list) {
        if (!bridge->known()) {
            continue;
        }

        json_array.append(bridge->dumpSyncbox());
    }

    QJsonDocument doc(json_array);

    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open syncbox save file.";
        return;
    }

    f.write(doc.toJson());
}

void HueSyncboxList::loadSyncboxes()
{
    QFile f(QStringLiteral("syncbox.json"));
    QJsonArray json_array;

    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open syncbox save file.";
        return;
    }

    QByteArray data = f.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    json_array = doc.array();

    for (int i = 0; i < json_array.size(); ++i) {
        QJsonObject syncbox = json_array[i].toObject();

        if (syncbox.contains("ipAddress") &&
            syncbox["ipAddress"].isString()) {

            QString ip = syncbox["ipAddress"].toString();

            createSyncbox(syncbox, ip);
        }
    }
}
