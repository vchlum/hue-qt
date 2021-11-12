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

#ifndef HUESYNCBOXLIST_H
#define HUESYNCBOXLIST_H

#include <QJsonObject>

#include "huelist.h"
#include "huesyncbox.h"

class HueSyncboxList : public HueList
{
    Q_OBJECT
    public:
        QList<HueSyncbox*> list;

        explicit HueSyncboxList(HueList *parent = 0);
        HueSyncbox* findSyncbox(QString s);
        HueSyncbox* addSyncbox(QString ip);
        void saveSyncboxes();
        void loadSyncboxes();

    private:

    signals:
        void syncboxDataUpdated();

    public slots:
        void createSyncbox(QJsonObject data, QString ip, bool on_load = false);

    private slots:
        void needSave();


};

#endif // HUESYNCBOXLIST_H