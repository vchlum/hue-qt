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

#ifndef MENUBRIDGE_H
#define MENUBRIDGE_H

#include <QVBoxLayout>

#include <huebridge.h>
#include <menuexpendable.h>

class BridgeLayout : public QVBoxLayout
{
    Q_OBJECT

    public:
        explicit BridgeLayout(HueBridge *showed_bridge, QWidget* parent = nullptr);

    protected:

    private:
        bool created = false;
        HueBridge *bridge;
        MenuExpendable* groups;
        MenuExpendable* lights;
        MenuExpendable* scenes;

        MenuExpendable* createGroups(QJsonObject json);
        MenuExpendable* createLights(QJsonObject json);
        MenuExpendable* createScenes(QJsonObject json);

    private slots:
        void updateBridge(QJsonObject json);
        void autoResize();

    signals:
        void sizeChanged();
};

#endif // MENUBRIDGE_H