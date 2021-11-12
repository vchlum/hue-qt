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

#ifndef HUEBRIDGELIST_H
#define HUEBRIDGELIST_H

#include <QJsonObject>

#include "huelist.h"
#include "huebridge.h"

class HueBridgeList : public HueList
{
    Q_OBJECT
    public:
        QList<HueBridge*> list;

        explicit HueBridgeList(HueList *parent = 0);
        HueBridge* findBridge(QString s);
        HueBridge* addBridge(QString ip);
        void checkBridges();
        void saveBridges();
        void loadBridges();

    private:

    signals:
        void bridgeDataUpdated();

    public slots:
        void createBridge(QJsonObject data, QString ip, bool on_load = false);

    private slots:
        void needSave();

};

#endif // HUEBRIDGELIST_H