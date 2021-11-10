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

#include <hueutils.h>

#include <menubutton.h>
#include "mainmenubridge.h"

BridgeLayout::BridgeLayout(HueBridge *showed_bridge, QWidget *parent): QVBoxLayout(parent)
{
    bridge = showed_bridge;
    connect(bridge, SIGNAL(statusV2(QJsonObject)), this, SLOT(updateBridge(QJsonObject)));

    setAlignment(Qt::AlignTop);

    bridge->getStatus2();
}

void BridgeLayout::autoResize() {
    emit sizeChanged();
}

MenuExpendable* BridgeLayout::createGroups(QJsonObject json)
{
    QList<QString> group_names = { "zone", "room" };
    groups = new MenuExpendable(300, 0);
    MenuLayout* item_layout;
    MenuButton* item;

    connect(groups, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    if (! json.contains("data")) {
        return groups;
    }

    item_layout = new MenuLayout(true, true);
    item_layout->setIcon(":images/HueIcons/devicesBridgesV2.svg");
    item_layout->setText(tr("All Rooms & Zones"));
    item = new MenuButton(*item_layout);
    groups->setHeadMenuButton(*item);

    QJsonArray data_array = json["data"].toArray();

    for (int n = 0; n  < group_names.size(); ++n) {
        for (int i = 0; i < data_array.size(); ++i) {
            QJsonObject j = data_array[i].toObject();
            if (j.contains("type") && j["type"].toString() == group_names.at(n)) {
                item_layout = new MenuLayout(true, true);
                item_layout->setText(j["metadata"].toObject()["name"].toString());

                item = new MenuButton(*item_layout);

                groups->addContentMenuButton(*item);
            }
        }
    }

    return groups;
}

MenuExpendable* BridgeLayout::createLights(QJsonObject json)
{
    lights = new MenuExpendable(300, 0);
    MenuLayout* item_layout;
    MenuButton* item;

    connect(lights, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    if (! json.contains("data")) {
        return lights;
    }

    item_layout = new MenuLayout(true, true);
    item_layout->setText(tr("All Lights"));
    item = new MenuButton(*item_layout);
    lights->setHeadMenuButton(*item);

    QJsonArray data_array = json["data"].toArray();

    for (int i = 0; i < data_array.size(); ++i) {
        QJsonObject j = data_array[i].toObject();
        if (j.contains("type") && j["type"].toString() == "light") {
            item_layout = new MenuLayout(true, true);
            item_layout->setText(j["metadata"].toObject()["name"].toString());

            item = new MenuButton(*item_layout);

            lights->addContentMenuButton(*item);
        }
    }

    return lights;
}

MenuExpendable* BridgeLayout::createScenes(QJsonObject json)
{
    scenes = new MenuExpendable(300, 0);
    MenuLayout* item_layout;
    MenuButton* item;

    connect(scenes, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    if (! json.contains("data")) {
        return scenes;
    }

    item_layout = new MenuLayout(false, false);
    item_layout->setText(tr("Scenes"));
    item = new MenuButton(*item_layout);
    scenes->setHeadMenuButton(*item);

    QJsonArray data_array = json["data"].toArray();

    for (int i = 0; i < data_array.size(); ++i) {
        QJsonObject j = data_array[i].toObject();
        if (j.contains("type") && j["type"].toString() == "scene") {
            item_layout = new MenuLayout(false, false);
            item_layout->setText(j["metadata"].toObject()["name"].toString());

            item = new MenuButton(*item_layout);

            scenes->addContentMenuButton(*item);
        }
    }

    return scenes;
}

void BridgeLayout::updateBridge(QJsonObject json)
{
    if (created) {

    } else {
        addWidget(createGroups(json));
        addWidget(createLights(json));
        addWidget(createScenes(json));
    }
}