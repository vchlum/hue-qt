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
#include <menucolorpicker.h>

#include "mainmenubridge.h"

BridgeWidget::BridgeWidget(HueBridge *showed_bridge, QWidget *parent): QWidget(parent)
{
    bridge = showed_bridge;
    connect(bridge, SIGNAL(statusV2(QJsonObject)), this, SLOT(updateBridge(QJsonObject)));

    QVBoxLayout* main_layout = new QVBoxLayout(this);

    main_layout->setAlignment(Qt::AlignTop);

    groups = new MenuExpendable(300, this);
    connect(groups, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(groups);

    lights = new MenuExpendable(300, this);
    connect(lights, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(lights);

    scenes = new MenuExpendable(300, this);
    connect(scenes, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(scenes);

    colors = new MenuExpendable(300, this);
    connect(colors, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(colors);

    bridge->getStatus2();
}

void BridgeWidget::autoResize()
{
    emit sizeChanged();
}

void BridgeWidget::groupClicked()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    selected_group = btn->id();

    selected_light = selected_group;

    // set* inplicates colapsing the expanded area
    setGroups(selected_group);
    setLights(selected_group, selected_light);
    setScenes(selected_group);

    lights->toggle(true);

    bridge->getStatus2();
}

void BridgeWidget::lightClicked()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    selected_light = btn->id();

    // set* inplicates colapsing the expanded area
    setLights(selected_group, selected_light);

    bridge->getStatus2();
}

void BridgeWidget::sceneClicked()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    selected_light = btn->id();

    bridge->getStatus2();
}

void BridgeWidget::updateButtonList(QString main_id)
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());

    refresh_button_list.remove(btn);
}

void BridgeWidget::setItemState(MenuButton* item, ItemState state, bool all_items_on)
{
    bool is_on = false;
    if (state.has_on) {
        if (all_items_on) {
            item->setSwitch(state.on_all);
            if (state.on_all) {
                is_on = true;
            }
        } else {
            item->setSwitch(state.on_any);
            if (state.on_any) {
                is_on = true;
            }
        }
    }

    if (state.has_dimming) {
        if (is_on) {
            item->setSlider(state.brightness);
        } else {
            item->setSlider(0);
        }
    }

    QColor off_color = QColor("#2E2E2E");
    if (state.has_color) {
        if (is_on) {
            item->setColor(state.color);
        } else {
            item->setColor(off_color);
        }
    } else {
        item->setColor(off_color);
    }

    item->setAllItems(all_items_on);
}

void BridgeWidget::setGroups(QString head_group_id)
{
    QString home_id;
    QString id;
    QList<QString> group_names = { "zone", "room" };
    MenuButton* item;
    QJsonArray data_array;
    QJsonObject json_item;
    ItemState state;

    if (! bridge_data.contains("data")) {
        return;
    }

    groups->clearContentButtons();

    data_array = getItemsByType(bridge_data, "bridge_home");
    home_id = data_array.at(0).toObject()["id"].toString();

    json_item = getItemById(bridge_data, head_group_id);

    state = getStateById(bridge_data, head_group_id);
    item = new MenuButton(head_group_id, state.has_dimming, state.has_on, groups);
    connect(item, SIGNAL(buttonRemoved(QString)), this, SLOT(updateButtonList(QString)));
    refresh_button_list[item] = state.ids;
    item->setIcon(":images/HueIcons/devicesBridgesV2.svg");

    if (head_group_id == home_id) {
        item->setText(tr("All Rooms & Zones"));
    } else {
        item->setText(json_item["metadata"].toObject()["name"].toString());
    }

    setItemState(item, state, false);
    groups->setHeadMenuButton(*item);

    // add home (all items) button
    if (head_group_id != home_id) {
        state = getStateById(bridge_data, home_id);
        item = new MenuButton(home_id, state.has_dimming, state.has_on, groups);
        connect(item, SIGNAL(buttonRemoved(QString)), this, SLOT(updateButtonList(QString)));
        refresh_button_list[item] = state.ids;
        item->setIcon(":images/HueIcons/devicesBridgesV2.svg");
        item->setText(tr("All Rooms & Zones"));
        connect(item, SIGNAL(clicked()), this, SLOT(groupClicked()));
        setItemState(item, state, false);
        groups->addContentMenuButton(*item);
    }

    for (int n = 0; n  < group_names.size(); ++n) {

        data_array = getItemsByType(bridge_data, group_names.at(n));

        for (int i = 0; i < data_array.size(); ++i) {

            json_item = data_array[i].toObject();
            id = json_item["id"].toString();

            if (id == head_group_id) {
                continue;
            }

            state = getStateById(bridge_data, id);
            item = new MenuButton(id, state.has_dimming, state.has_on, groups);
            connect(item, SIGNAL(buttonRemoved(QString)), this, SLOT(updateButtonList(QString)));
            refresh_button_list[item] = state.ids;
            item->setText(json_item["metadata"].toObject()["name"].toString());
            connect(item, SIGNAL(clicked()), this, SLOT(groupClicked()));
            setItemState(item, state, false);
            groups->addContentMenuButton(*item);
        }
    }

    return;
}

void BridgeWidget::setLights(QString group_id, QString head_light_id)
{
    QJsonObject json_item;
    QJsonObject json_item_group;
    QString group_type;
    MenuButton* item;
    QJsonArray services_array;
    QString rid;
    int counter = 0;
    ItemState state;

    if (! bridge_data.contains("data")) {
        return;
    }

    lights->clearContentButtons();

    json_item_group = getItemById(bridge_data, group_id);
    json_item = getItemById(bridge_data, head_light_id);
    group_type = getTypeById(bridge_data, group_id);

    state = getStateById(bridge_data, head_light_id);
    item = new MenuButton(head_light_id, group_id == head_light_id ? false : state.has_dimming, state.has_on, lights);
    connect(item, SIGNAL(buttonRemoved(QString)), this, SLOT(updateButtonList(QString)));
    refresh_button_list[item] = state.ids;
    if (group_id == head_light_id) {
        item->setText(tr("All Lights"));
        setItemState(item, state, true);
    } else {
        item->setText(json_item["metadata"].toObject()["name"].toString());
        setItemState(item, state, false);
    }
    lights->setHeadMenuButton(*item);

    if (group_id != head_light_id) {
        state = getStateById(bridge_data, group_id);
        item = new MenuButton(group_id, false, state.has_on, lights);
        connect(item, SIGNAL(buttonRemoved(QString)), this, SLOT(updateButtonList(QString)));
        refresh_button_list[item] = state.ids;
        item->setText(tr("All Lights"));
        connect(item, SIGNAL(clicked()), this, SLOT(lightClicked()));
        setItemState(item, state, true);
        lights->addContentMenuButton(*item);

        counter++;
    }

    services_array = getServicesById(bridge_data, group_id);

    for (int i = 0; i < services_array.size(); ++i) {
        json_item = services_array[i].toObject();

        if (json_item["rtype"].toString() != "light") {
            continue;
        }

        rid = json_item["rid"].toString();

        if (rid == head_light_id) {
            continue;
        }

        json_item = getItemById(bridge_data, rid);

        state = getStateById(bridge_data, rid);
        item = new MenuButton(rid, state.has_dimming, state.has_on, lights);
        connect(item, SIGNAL(buttonRemoved(QString)), this, SLOT(updateButtonList(QString)));
        refresh_button_list[item] = state.ids;
        item->setText(json_item["metadata"].toObject()["name"].toString());
        connect(item, SIGNAL(clicked()), this, SLOT(lightClicked()));
        setItemState(item, state, false);
        lights->addContentMenuButton(*item);

        counter++;
    }

    if (counter == 0) {
        lights->setVisible(false);
    } else {
        lights->setVisible(true);
    }

    return;
}

void BridgeWidget::setScenes(QString head_id)
{
    QString id;
    MenuButton* item;
    QJsonArray data_array;
    int counter = 0;

    if (! bridge_data.contains("data")) {
        return;
    }

    scenes->clearContentButtons();

    data_array = getItemsByType(bridge_data, "bridge_home");
    id = data_array.at(0).toObject()["id"].toString();

    item = new MenuButton(id, false, false, scenes);
    item->setText(tr("Scenes"));
    scenes->setHeadMenuButton(*item);

    data_array = getItemsByType(bridge_data, "scene");

    for (int i = 0; i < data_array.size(); ++i) {
        QJsonObject json_item = data_array[i].toObject();
        if (json_item["group"].toObject()["rid"].toString() != head_id) {
            continue;
        }

        id = json_item["id"].toString();

        item = new MenuButton(id, false, false, scenes);
        item->setText(json_item["metadata"].toObject()["name"].toString());
        connect(item, SIGNAL(clicked()), this, SLOT(sceneClicked()));
        scenes->addContentMenuButton(*item);

        counter++;
    }

    if (counter == 0) {
        scenes->setVisible(false);
    } else {
        scenes->setVisible(true);
    }

    return;
}

void BridgeWidget::setColorsTemperature(QString group_id, QString light_id)
{
    MenuButton* item;

    if (! bridge_data.contains("data")) {
        return;
    }

    item = new MenuButton(light_id, false, false, colors);
    item->setText(tr("Colors & Temperature"));
    colors->setHeadMenuButton(*item);

    ColorPicker *color_picker = new ColorPicker();
    colors->setContentWidget(*color_picker);
}

void BridgeWidget::updateAllButtons()
{
    ItemState state;
    MenuButton* btn;

    QMapIterator<MenuButton*, QVarLengthArray<QString>> i(refresh_button_list);
    while (i.hasNext()) {
        i.next();

        btn = i.key();
        state = getStateById(bridge_data, btn->id());
        setItemState(btn, state, btn->allItems());
    }
}

void BridgeWidget::updateBridge(QJsonObject json)
{
    QJsonArray data_array;

    bridge_data = json;

    if (selected_group == "") {
        data_array = getItemsByType(bridge_data, "bridge_home");
        selected_group = data_array.at(0).toObject()["id"].toString();
    }

    if (selected_light == "") {
        selected_light = selected_group;
    }

    if (rebuild) {
        setGroups(selected_group);
        setLights(selected_group, selected_light);
        setScenes(selected_group);
        setColorsTemperature(selected_group, selected_light);
    } else {
        updateAllButtons();
    }

    rebuild = false;
}