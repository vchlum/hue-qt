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

#include <hueutils.h>

#include <menubutton.h>
#include <menucolorpicker.h>
#include <menuutils.h>

#include "mainmenubridge.h"

BridgeWidget::BridgeWidget(HueBridge *showed_bridge, QWidget *parent): QWidget(parent)
{
    bridge = showed_bridge;
    connect(bridge, SIGNAL(statusV2(QJsonObject)), this, SLOT(updateBridge(QJsonObject)));
    connect(bridge, SIGNAL(events(QJsonArray&)), this, SLOT(processEvents(QJsonArray&)));

    QVBoxLayout* main_layout = new QVBoxLayout(this);

    main_layout->setAlignment(Qt::AlignTop);

    groups = new MenuExpendable(300, this);
    connect(groups, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(groups);

    lights = new MenuExpendable(300, this);
    connect(lights, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(lights);

    colors = new MenuExpendable(300, this);
    connect(colors, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(colors);

    scenes = new MenuExpendable(300, this);
    connect(scenes, SIGNAL(menuToggled()), this, SLOT(autoResize()));
    main_layout->addWidget(scenes);

    bridge->getStatus();
}

void BridgeWidget::autoResize()
{
    emit sizeChanged();
}

void BridgeWidget::groupClicked()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    selected_group = btn->id();

    selected_light = "";

    // set* inplicates colapsing the expanded area
    setLights(selected_group);
    setColorsTemperature(selected_light, selected_group);
    setScenes(selected_group);

    groups->toggle(false);
    lights->toggle(true);
    colors->toggle(false);
    scenes->toggle(false);
}

void BridgeWidget::lightClicked()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    selected_light = btn->id();

    // set* inplicates colapsing the expanded area
    setColorsTemperature(selected_light, selected_group);

    groups->toggle(false);
    lights->toggle(false);
    colors->toggle(true);
    scenes->toggle(false);
}

void BridgeWidget::sceneClicked()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    selected_light = btn->id();

    QJsonObject json;

    QJsonObject json_on;
    json_on["action"] = "active";
    json["recall"] = json_on;

    bridge->putScene(btn->id(), json);
}

void BridgeWidget::removeFromButtonList()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());

    refresh_button_list.remove(btn);
}

void BridgeWidget::switchId(QString id, bool on)
{
    QJsonObject json;

    QJsonObject json_on;
    json_on["on"] = on;
    json["on"] = json_on;

    if (states_groups.contains(id)) {
        bridge->putGroupedLight(states_groups[id].grouped_light_rid, json);
    } else {
        bridge->putLight(id, json);
    }
}

void BridgeWidget::dimmId(QString id, int value)
{
    QJsonObject json;

    QJsonObject json_on;
    json_on["on"] = true;
    json["on"] = json_on;

    QJsonObject json_brightness;
    json_brightness["brightness"] = value;
    json["dimming"] = json_brightness;

    if (states_groups.contains(id)) {
        bool any_on = checkAnyServiceIsOn(states_groups[id].services, "light");

        QMapIterator<QString, QString> service(states_groups[id].services);
        while (service.hasNext()) {
            service.next();

            if (service.value() == "light" && (states_lights[service.key()].on || !any_on)) {
                delay(bridge_delay);
                bridge->putLight(service.key(), json);
            }
        }

        // the bridge does not allow to change the brightness of group yet
        // bridge->putGroupedLight(states_groups[id].grouped_light_rid, json);
    } else {
        bridge->putLight(id, json);
    }
}

void BridgeWidget::changeColorGradient(QString id, QColor color)
{
    QJsonObject json;
    int gradient_point;
    ItemState state;

    state = states_lights[id];

    if (!state.has_gradient) {
        return;
    }

    QVarLengthArray<float> xy = colorToHueXY(color);

    ColorPicker *cpck = qobject_cast<ColorPicker *>(sender());

    gradient_point = cpck->property("gradient_point").toInt();

    QJsonObject json_on;
    json_on["on"] = true;
    json["on"] = json_on;

    QJsonObject json_gradient;
    QJsonArray points_array;
    for (int i = 0; i < state.gradient_points_capable; ++i) {
        QVarLengthArray<float> tmp_xy;

        if (gradient_point == i) {
            tmp_xy = xy;
        } else if (state.gradient_points.length() != state.gradient_points_capable) {
            tmp_xy = xy;
        } else {
            tmp_xy = colorToHueXY(state.gradient_points[i]);
        }

        QJsonObject json_xy;
        json_xy["x"] = tmp_xy[0];
        json_xy["y"] = tmp_xy[1];

        QJsonObject json_color_xy;
        json_color_xy["xy"] = json_xy;

        QJsonObject json_color;
        json_color["color"] = json_color_xy;

        points_array.append(json_color);
    }
    json_gradient["points"] = points_array;
    json["gradient"] = json_gradient;

    bridge->putLight(id, json);
}

void BridgeWidget::changeColor(QString id, QColor color)
{
    QJsonObject json;
    QVarLengthArray<float> xy = colorToHueXY(color);

    QJsonObject json_on;
    json_on["on"] = true;
    json["on"] = json_on;

    QJsonObject json_xy;
    json_xy["x"] = xy[0];
    json_xy["y"] = xy[1];

    QJsonObject json_color;
    json_color["xy"] = json_xy;
    json["color"] = json_color;

    if (states_groups.contains(id)) {
        bool any_on = checkAnyServiceIsOn(states_groups[id].services, "light");

        QMapIterator<QString, QString> service(states_groups[id].services);
        while (service.hasNext()) {
            service.next();

            if (service.value() == "light" && (states_lights[service.key()].on || !any_on)) {
                delay(bridge_delay);
                bridge->putLight(service.key(), json);
            }
        }

        // the bridge does not allow to change the brightness of group yet
        // bridge->putGroupedLight(states_groups[id].grouped_light_rid, json);
    } else {
        bridge->putLight(id, json);
    }
}

void BridgeWidget::changeMirek(QString id, int mirek)
{
    QJsonObject json;

    QJsonObject json_on;
    json_on["on"] = true;
    json["on"] = json_on;

    QJsonObject json_mirek;
    json_mirek["mirek"] = mirek;
    json["color_temperature"] = json_mirek;

    if (states_groups.contains(id)) {
        bool any_on = checkAnyServiceIsOn(states_groups[id].services, "light");

        QMapIterator<QString, QString> service(states_groups[id].services);
        while (service.hasNext()) {
            service.next();

            if (service.value() == "light" && (states_lights[service.key()].on || !any_on)) {
                delay(bridge_delay);
                bridge->putLight(service.key(), json);
            }
        }

        // the bridge does not allow to change the brightness of group yet
        // bridge->putGroupedLight(states_groups[id].grouped_light_rid, json);
    } else {
        bridge->putLight(id, json);
    }
}

void BridgeWidget::gradientSelected(QString id, int point)
{
    selected_light = id;
    setColorsTemperature(selected_light, selected_group, point);

    groups->toggle(false);
    lights->toggle(false);
    colors->toggle(true);
    scenes->toggle(false);
}

void BridgeWidget::addGroupState(QJsonObject json)
{
    QString id;

    if (! json.contains("id")) {
        return;
    }

    id = json["id"].toString();

    if (json["type"].toString() == "bridge_home") {
        bridge_home_id = id;
    }

    states_groups[id] = getGroupFromJson(json);
}

void BridgeWidget::addLightState(QJsonObject json)
{
    QString id;

    if (! json.contains("id")) {
        return;
    }

    id = json["id"].toString();
    states_lights[id] = getLightFromJson(json);
}

void BridgeWidget::addSceneState(QJsonObject json)
{
    QString id;

    if (! json.contains("id")) {
        return;
    }

    id = json["id"].toString();
    states_scenes[id] = getSceneFromJson(json);
}

QMap<QString, ItemState>* BridgeWidget::getStatesByType(QString type)
{
    if (type == "bridge_home")
        return &states_groups;

    else if (type == "room")
        return &states_groups;

    else if (type == "zone")
        return &states_groups;

    else if (type == "light")
        return &states_lights;

    else if (type == "scene")
        return &states_scenes;

    return NULL;
}

void BridgeWidget::createStates(QJsonObject json)
{
    QJsonObject json_item;
    QString type;

    states_groups.clear();
    states_lights.clear();
    states_scenes.clear();

    if (! json.contains("data")) {
        return;
    }

    QJsonArray json_array = json["data"].toArray();

    for (int i = 0; i < json_array.size(); ++i) {
        QJsonObject json_item = json_array[i].toObject();

        if (! json_item.contains("type")) {
            continue;
        }

        type = json_item["type"].toString();

        if (type == "bridge_home")
            addGroupState(json_item);

        else if (type == "room")
            addGroupState(json_item);

        else if (type == "zone")
            addGroupState(json_item);

        else if (type == "light")
            addLightState(json_item);

        else if (type == "scene")
            addSceneState(json_item);
    }
}

void BridgeWidget::updateButtonState(MenuButton* button, ItemState state)
{
    if (state.dummy) {
        return;
    }

    bool is_on = false;
    bool is_all_on = false;

    if (state.has_on && button->combinedAll()) {
        is_all_on = checkAllServicesAreOn(state.services, "light");
        button->setSwitch(is_all_on);
        is_on = state.on;
    } else if (state.has_on) {
        is_on = state.on;
        button->setSwitch(is_on);
    }

    if (state.has_dimming) {
        if (is_on) {
            button->setSlider(state.brightness);
        } else {
            button->setSlider(0);
        }
    }

    QColor off_color = QColor("#2E2E2E");
    if (state.has_color) {
        if (is_on) {
            button->setColor(state.color);
        } else {
            button->setColor(off_color);
        }
    } else if (state.has_mirek) {
        if (is_on) {
            button->setColor(state.mirek_color);
        } else {
            button->setColor(off_color);
        }
    } else {
        button->setColor(off_color);
    }

    if (state.has_gradient && state.gradient_points_capable > 0) {
        if (!is_on || state.gradient_points.length() == 0) {
            QVarLengthArray<QColor> gradient_points;

            for (int i = 0; i < state.gradient_points_capable; ++i) {
                gradient_points.append(off_color);
            }

            button->setColorsPoints(gradient_points);
        } else {
            button->setColorsPoints(state.gradient_points);
        }
    }

    if (button->id() == "" || state.type == "")
        throw;

    refresh_button_list[button] = state.type;
}

MenuButton* BridgeWidget::createMenuButton(
    MenuExpendable* menu,
    ItemState state,
    void (BridgeWidget::*button_slot)(),
    bool back_button,
    QString custom_text,
    QString custom_icon,
    bool combined,
    bool combined_all)
{
    QString id;
    MenuButton* button;

    id = state.id;

    button = new MenuButton(id, state.has_dimming, state.gradient_points_capable, back_button, state.has_on, menu);

    if (!state.dummy) {
        connect(button, SIGNAL(buttonRemoved(QString)), this, SLOT(removeFromButtonList()));
    }

    if (custom_icon != "") {
        button->setIcon(custom_icon);
    } else if (state.archetype != "" && icon_list.contains(state.archetype)) {
        button->setIcon(icon_list[state.archetype]);
    } else {
        button->setIcon(":images/HueIcons/devicesBridgesV2.svg");
    }

    if (custom_text != "") {
        button->setText(custom_text);
    } else {
        button->setText(state.name);
    }

    if (button_slot != NULL) {
        connect(button, &MenuButton::clicked, this, button_slot);
    }

    if (state.has_on) {
        connect(button, SIGNAL(switched(QString, bool)), this, SLOT(switchId(QString, bool)));
    }

    if (state.has_dimming) {
        connect(button, SIGNAL(dimmed(QString, int)), this, SLOT(dimmId(QString, int)));
    }

    if (state.gradient_points_capable > 0) {
        connect(button, SIGNAL(pointed(QString, int)), this, SLOT(gradientSelected(QString, int)));
    }

    button->setCombined(combined, combined_all);

    updateButtonState(button, state);

    return button;
}

bool BridgeWidget::checkAnyServiceIsOn(QMapIterator<QString, QString> services, QString type)
{
    QString id;

    QMapIterator<QString, QString> service(services);
    while (service.hasNext()) {
        service.next();

        id = service.key();

        if (service.value() != type) {
            continue;
        }

        if (states_lights[id].has_on && states_lights[id].on) {
            return true;
        }
    }

    return false;
}

bool BridgeWidget::checkAllServicesAreOn(QMapIterator<QString, QString> services, QString type)
{
    QString id;

    QMapIterator<QString, QString> service(services);
    while (service.hasNext()) {
        service.next();

        id = service.key();

        if (service.value() != type) {
            continue;
        }

        if (states_lights[id].has_on && !states_lights[id].on) {
            return false;
        }
    }

    return true;
}

ItemState BridgeWidget::getCombinedGroupState(ItemState base_state)
{
    ItemState state = base_state;
    QString id;
    QString type;

    QMapIterator<QString, QString> service(state.services);
    while (service.hasNext()) {
        service.next();

        id = service.key();
        type = service.value();

        if (type != "light") {
            continue;
        }

        state = combineTwoStates(state, states_lights[id]);
    }

    state.services = base_state.services;

    return state;
}

void BridgeWidget::setGroups()
{
    QString id;
    ItemState state;
    MenuButton* button;
    int counter = 0;

    groups->clearContentButtons();

    id = bridge_home_id;
    state = states_groups[id];
    state = getCombinedGroupState(state);
    button = createMenuButton(groups, state, NULL, false, bridge->deviceName(), ":images/HueIcons/roomsOther.svg", true);
    groups->setHeadMenuButton(*button);

    connect(button, &MenuButton::clicked, [this]() {
        lights->toggle(false);
        colors->toggle(false);
        scenes->toggle(false);
    });

    QMapIterator<QString, ItemState> item(states_groups);
    while (item.hasNext()) {
        item.next();

        id = item.key();
        state = item.value();

        if (id == bridge_home_id) {
            continue;
        }

        state = getCombinedGroupState(state);
        button = createMenuButton(groups, state, &BridgeWidget::groupClicked, false, "", "", true);
        groups->addContentMenuButton(*button);

        counter++;
    }

    if (counter == 0) {
        groups->setVisible(false);
    } else {
        groups->setVisible(true);
    }
}

void BridgeWidget::setLights(QString group_id)
{
    QString id;
    ItemState state;
    ItemState state_selected_group;
    bool group_selected;
    MenuButton* button;
    QString button_text;
    QString button_icon;
    int counter = 0;

    lights->clearContentButtons();

    if (group_id == bridge_home_id){
        button_text = tr("All lights");
        button_icon = ":images/HueIcons/bulbGroup.svg";
    } else {
        button_text = "";
        button_icon = "";
        group_selected = true;
    }

    state = states_groups[group_id];
    state = getCombinedGroupState(state);
    button = createMenuButton(lights, state, NULL, group_selected, button_text, button_icon, true);
    lights->setHeadMenuButton(*button);

    connect(button, &MenuButton::clicked, [this]() {
        groups->toggle(false);
        colors->toggle(false);
        scenes->toggle(false);
    });

    if (group_selected) {
        connect(button, &MenuButton::goBack, [this]() {

            selected_group = bridge_home_id;
            selected_light = "";

            // set* inplicates colapsing the expanded area
            setLights(selected_group);
            setColorsTemperature(selected_light, selected_group);
            setScenes(selected_group);

            groups->toggle(true);
            lights->toggle(false);
            colors->toggle(false);
            scenes->toggle(false);
        });
    }

    state_selected_group = states_groups[group_id];

    QMapIterator<QString, ItemState> item(states_lights);
    while (item.hasNext()) {
        item.next();

        id = item.key();

        if (! state_selected_group.services.contains(id)) {
            continue;
        }

        if (state_selected_group.services[id] != "light") {
            continue;
        }

        state = item.value();
        button = createMenuButton(lights, state, &BridgeWidget::lightClicked, false);
        lights->addContentMenuButton(*button);

        counter++;
    }

    if (counter == 0) {
        lights->setVisible(false);
    } else {
        lights->setVisible(true);
    }
}

void BridgeWidget::setColorsTemperature(QString light_id, QString group_id, int gradient_point)
{
    QString id;
    MenuButton* button;
    ItemState state;
    QString button_text;
    bool light_selected = false;

    if (group_id == bridge_home_id && light_id == ""){
        button_text = tr("All lights");
    } else {
        button_text = "";

        if (light_id != "")
            light_selected = true;
    }

    id = light_id == "" ? group_id : light_id;

    state = light_id == "" ? states_groups[id] : states_lights[id] ;

    if (light_id == "") {
        state = getCombinedGroupState(state);
    }

    if (light_id != "" && gradient_point > -1) {
        button_text = QString("%1 - %2 %3").arg(state.name).arg(tr("segment")).arg(gradient_point + 1);
    }

    if (light_id == "") {
        button = createMenuButton(colors, state, NULL, light_selected, button_text, ":images/HueIcons/uicontrolsColorScenes.svg", true, true);
    } else {
        button = createMenuButton(colors, state, NULL, light_selected, button_text);
    }

    colors->setHeadMenuButton(*button);

    connect(button, &MenuButton::clicked, [this]() {
        groups->toggle(false);
        lights->toggle(false);
        scenes->toggle(false);
    });

    if (light_selected) {
        connect(button, &MenuButton::goBack, [this]() {

            if (selected_light != "") {
                selected_light = "";
            } else {
                selected_group = bridge_home_id;
            }

            // set* inplicates colapsing the expanded area
            setLights(selected_group);
            setColorsTemperature(selected_light, selected_group);
            setScenes(selected_group);
            if (selected_group == bridge_home_id) {
                groups->toggle(true);
                lights->toggle(false);
            } else {
                groups->toggle(false);
                lights->toggle(true);
            }
            colors->toggle(false);
            scenes->toggle(false);
        });
    }

    bool use_temperature = gradient_point == -1 ? state.has_mirek : false;

    ColorPicker *color_picker = new ColorPicker(id, state.has_color, use_temperature, this);

    if (gradient_point >= 0) {
        color_picker->setProperty("gradient_point", gradient_point);
        connect(color_picker, SIGNAL(colorPicked(QString, QColor)), this, SLOT(changeColorGradient(QString, QColor)));
    } else {
        connect(color_picker, SIGNAL(colorPicked(QString, QColor)), this, SLOT(changeColor(QString, QColor)));
    }

    connect(color_picker, SIGNAL(mirekPicked(QString, int)), this, SLOT(changeMirek(QString, int)));

    colors->setContentWidget(*color_picker);
}

void BridgeWidget::setScenes(QString group_id)
{
    QString id;
    QString button_text;
    MenuButton* button;
    ItemState state;
    int counter = 0;

    scenes->clearContentButtons();

    if (group_id == bridge_home_id) {
        scenes->setVisible(false);
        return;
    } else {
        scenes->setVisible(true);
    }

    state.dummy = true;

    if (group_id == bridge_home_id) {
        button_text = tr("Scenes");
    } else {
        button_text = states_groups[group_id].name;
    }

    button = createMenuButton(scenes, state, NULL, false, button_text, ":images/HueIcons/uicontrolsScenes.svg");
    scenes->setHeadMenuButton(*button);

    connect(button, &MenuButton::clicked, [this]() {
        groups->toggle(false);
        lights->toggle(false);
        colors->toggle(false);
    });

    QMapIterator<QString, ItemState> item(states_scenes);
    while (item.hasNext()) {
        item.next();

        id = item.key();
        state = item.value();

        if (state.group_id != group_id) {
            continue;
        }

        button = createMenuButton(scenes, state, &BridgeWidget::sceneClicked, false, "", ":images/HueIcons/uicontrolsScenes.svg");
        scenes->addContentMenuButton(*button);

        counter++;
    }

    if (counter == 0) {
        scenes->setVisible(false);
    } else {
        scenes->setVisible(true);
    }

    return;
}

void BridgeWidget::updateAllButtons()
{
    ItemState state;
    MenuButton* btn;
    QString type;
    QMap<QString, ItemState> *states;

    QMapIterator<MenuButton*, QString> i(refresh_button_list);
    while (i.hasNext()) {
        i.next();

        btn = i.key();
        type = i.value();

        states = getStatesByType(type);
        state = (*states)[btn->id()];

        updateButtonState(btn, state);
    }
}

void BridgeWidget::updateBridge(QJsonObject json)
{
    createStates(json);

    if (selected_group == "") {
        selected_group = bridge_home_id;
    }

    if (rebuild) {
        setGroups();
        setLights(selected_group);
        setColorsTemperature(selected_light, selected_group);
        setScenes(selected_group);

        groups->toggle(true);
        lights->toggle(false);
        colors->toggle(false);
        scenes->toggle(false);
    } else {
        updateAllButtons();
    }

    rebuild = false;
}

QString BridgeWidget::updateStateByEvent(QJsonObject json)
{
    QString id;
    QString type;
    QMap<QString, ItemState> *states;
    ItemState state;

    if (!json.contains("id")) {
        return "";
    }

    id = json["id"].toString();
    type = json["type"].toString();

    states = getStatesByType(type);
    state = (*states)[id];

    state = updateState(state, json);

    (*states)[id] = state;

    return id;
}

void BridgeWidget::updateRelatedButtons()
{
    MenuButton* button;
    QString type;
    ItemState state;
    QMap<QString, ItemState> *states;

    QMapIterator<MenuButton*, QString> i(refresh_button_list);
    while (i.hasNext()) {
        i.next();

        button = i.key();
        type = i.value();

        if (events_update_list.contains(button->id())) {
            states = getStatesByType(type);
            state = (*states)[button->id()];
            updateButtonState(button, state);
        }

        if (button->combined()) {            
            states = getStatesByType(type);
            state = (*states)[button->id()];

            bool affected = false;

            QMapIterator<QString, QString> j(state.services);
            while (j.hasNext()) {
                j.next();

                if (events_update_list.contains(j.key())) {
                    affected = true;
                    break;
                }
            }

            if (affected) {
                state = getCombinedGroupState(state);
                updateButtonState(button, state);
            }
        }
    }

    events_update_list.clear();
    waiting_events = false;
}

void BridgeWidget::processEvents(QJsonArray &json_array)
{
    QJsonObject json;
    QString updated;

    for (int i = 0; i < json_array.size(); ++i) {
        json = json_array[i].toObject();

        if (! json.contains("data")) {
            continue;
        }

        if (json["type"].toString() != "update") {
            continue;
        }

        QJsonArray json_event_array = json["data"].toArray();
        for (int j = 0; j < json_event_array.size(); ++j) {
            QJsonObject json_event = json_event_array[j].toObject();

            if (json_event["type"].toString() == "light") {
                updated = updateStateByEvent(json_event);
                events_update_list.append(updated);
            }
        }
    }

    if (waiting_events == false) {
        TimerThread *timer = new TimerThread();
        connect(timer, &TimerThread::timeIsUp, this, &BridgeWidget::updateRelatedButtons );
        connect(timer, &TimerThread::finished, timer, &QObject::deleteLater);
        timer->setDelay(1200);
        timer->start();
    }

    waiting_events = true;
}
