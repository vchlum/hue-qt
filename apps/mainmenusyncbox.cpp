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

#include "mainmenusyncbox.h"

SyncboxWidget::SyncboxWidget(HueSyncbox *showed_syncbox, QWidget *parent): QWidget(parent)
{
    syncbox = showed_syncbox;
    connect(syncbox, SIGNAL(status(QJsonObject)), this, SLOT(updateSyncbox(QJsonObject)));
    connect(syncbox, SIGNAL(executionFinished()), this, SLOT(checkSyncbox()));

    QVBoxLayout* main_layout = new QVBoxLayout(this);

    main_layout->setAlignment(Qt::AlignTop);

    main_btn = new MenuButton("", false, -1, false, true, this);
    main_btn->setText(syncbox->deviceName());
    main_btn->setIcon(":images/syncbox.svg");
    connect(main_btn, SIGNAL(switched(QString, bool)), this, SLOT(changePower(QString, bool)));
    main_layout->addWidget(main_btn);

    modes = new MenuExpendable(300, this);
    connect(modes, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    main_layout->addWidget(modes);

    intensities = new MenuExpendable(300, this);
    connect(intensities, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    main_layout->addWidget(intensities);

    brightness_btn = new MenuButton("", true, -1, false, false, this);
    connect(brightness_btn, SIGNAL(dimmed(QString, int)), this, SLOT(changeBrightness(QString, int)));
    brightness_btn->setText(tr("Brightness"));
    brightness_btn->setSliderMax(200);
    brightness_btn->setIcon(":images/HueIcons/routinesDaytime.svg");
    main_layout->addWidget(brightness_btn);

    hdmies = new MenuExpendable(300, this);
    connect(hdmies, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    main_layout->addWidget(hdmies);

    groups = new MenuExpendable(300, this);
    connect(groups, SIGNAL(menuToggled()), this, SLOT(autoResize()));

    main_layout->addWidget(groups);

    syncbox->getStatus();
}

void SyncboxWidget::autoResize()
{
    emit sizeChanged();
}

void SyncboxWidget::checkSyncbox()
{
    syncbox->getStatus();
}

void SyncboxWidget::changePower(QString id, bool on)
{
    syncbox->setPower(on);
}

void SyncboxWidget::changeBrightness(QString id, int value)
{
    syncbox->setBrightness(value);
}

void SyncboxWidget::changeSync(QString id, bool on)
{
    syncbox->setSync(on);
}

void SyncboxWidget::changeMode()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    syncbox->setMode(btn->property("mode").toString());
}

void SyncboxWidget::changeIntensity()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    syncbox->setIntensity(btn->property("intensity").toString());
}

void SyncboxWidget::changeInput()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    syncbox->setInput(btn->property("input").toString());
}

void SyncboxWidget::changeGroup()
{
    MenuButton *btn = qobject_cast<MenuButton *>(sender());
    syncbox->setGroup(btn->property("group").toString());
}

void SyncboxWidget::createModes()
{
    MenuButton* mode_button;

    mode_button = new MenuButton("", false, -1, false, true, this);
    mode_button->setText(mode_list[state.last_sync_mode]);
    mode_button->setSwitch(state.sync_active);
    connect(mode_button, SIGNAL(switched(QString, bool)), this, SLOT(changeSync(QString, bool)));

    connect(mode_button, &MenuButton::clicked, [this]() {
        intensities->toggle(false);
        hdmies->toggle(false);
        groups->toggle(false);
    });

    mode_btn = mode_button;

    modes->setHeadMenuButton(*mode_button);

    foreach (const QString& key, state.mode_list.keys()) {
        mode_button = new MenuButton("", false, -1, false, false, this);
        connect(mode_button, SIGNAL(clicked()), this, SLOT(changeMode()));
        mode_button->setText(mode_list[key]);
        mode_button->setProperty("mode", key);
        mode_button->setIcon(mode_icon[key]);

        modes->addContentMenuButton(*mode_button);
    }
}

void SyncboxWidget::createIntensities()
{
    MenuButton* intens_button;
    QString intensity;

    intensity = state.mode_list[state.last_sync_mode];

    intens_button = new MenuButton("", false, -1, false, false, this);
    intens_button->setText(tr("Intensity") + ": "+ intensity_list[intensity]);

    connect(intens_button, &MenuButton::clicked, [this]() {
        modes->toggle(false);
        hdmies->toggle(false);
        groups->toggle(false);
    });

    intens_btn = intens_button;

    intensities->setHeadMenuButton(*intens_button);

    for (int i = 0; i < intensity_ordered.length(); ++i) {

        QString key = intensity_ordered[i];

        intens_button = new MenuButton("", false, -1, false, false, this);
        connect(intens_button, SIGNAL(clicked()), this, SLOT(changeIntensity()));
        intens_button->setText(intensity_list[key]);
        intens_button->setProperty("intensity", key);
        intens_button->setIcon(":images/intensity-" + key+ ".svg");

        intensities->addContentMenuButton(*intens_button);
    }
}

void SyncboxWidget::createHdmies()
{
    MenuButton* hdmi_button;
    QString intensity;

    intensity = state.mode_list[state.last_sync_mode];

    hdmi_button = new MenuButton("", false, -1, false, false, this);
    hdmi_button->setText(state.hdmies_name_list[state.hdmi_source]);
    hdmi_button->setIcon(":images/hdmi.svg");

    connect(hdmi_button, &MenuButton::clicked, [this]() {
        modes->toggle(false);
        intensities->toggle(false);
        groups->toggle(false);
    });

    hdmi_btn = hdmi_button;

    hdmies->setHeadMenuButton(*hdmi_button);

    foreach (const QString& key, state.hdmies_name_list.keys()) {

        hdmi_button = new MenuButton("", false, -1, false, false, this);
        connect(hdmi_button, SIGNAL(clicked()), this, SLOT(changeInput()));
        hdmi_button->setText(state.hdmies_name_list[key]);
        hdmi_button->setProperty("input", key);
        hdmi_button->setIcon(":images/hdmi.svg");

        hdmies->addContentMenuButton(*hdmi_button);
    }
}

void SyncboxWidget::createGroups()
{
    MenuButton* group_button;
    QString intensity;

    intensity = state.mode_list[state.last_sync_mode];

    group_button = new MenuButton("", false, -1, false, false, this);
    group_button->setText(state.groups_list[state.groupid]);
    group_button->setIcon(":images/HueIcons/roomsMancave.svg");

    connect(group_button, &MenuButton::clicked, [this]() {
        modes->toggle(false);
        intensities->toggle(false);
        hdmies->toggle(false);
    });

    group_btn = group_button;

    groups->setHeadMenuButton(*group_button);

    foreach (const QString& key, state.groups_list.keys()) {

        group_button = new MenuButton("", false, -1, false, false, this);
        connect(group_button, SIGNAL(clicked()), this, SLOT(changeGroup()));
        group_button->setText(state.groups_list[key]);
        group_button->setProperty("group", key);
        group_button->setIcon(":images/HueIcons/roomsMancave.svg");

        groups->addContentMenuButton(*group_button);
    }
}

void SyncboxWidget::setMainBtn()
{
    main_btn->setText(state.name);

    if (state.mode == "powersave") {
        main_btn->setSwitch(false);
    } else {
        main_btn->setSwitch(true);
    }
}

void SyncboxWidget::setModes()
{
    mode_btn->setText(mode_list[state.last_sync_mode]);
    mode_btn->setSwitch(state.sync_active);
    mode_btn->setIcon(mode_icon[state.last_sync_mode]);
}

void SyncboxWidget::setIntensities()
{
    QString intensity = state.mode_list[state.last_sync_mode];
    intens_btn->setText(tr("Intensity") + ": " + intensity_list[intensity]);
    intens_btn->setIcon(":images/intensity-" + intensity + ".svg");
}

void SyncboxWidget::setBrightness()
{
    brightness_btn->setSlider(state.brightness);
}

void SyncboxWidget::setHdmies()
{
    hdmi_btn->setText(state.hdmies_name_list[state.hdmi_source]);
}

void SyncboxWidget::setGroups()
{
    group_btn->setText(state.groups_list[state.groupid]);
}

void SyncboxWidget::updateState(QJsonObject json)
{
    QJsonObject json_item;
    QString type;

    if (json.contains("device")) {
        json_item = json["device"].toObject();

        state.name = json_item["name"].toString();
        state.id = json_item["uniqueId"].toString();
        state.wifi_ssid = json_item["wifi"].toObject()["ssid"].toString();
    }

    if (json.contains("hue")) {
        json_item = json["hue"].toObject();

        state.groupid = json_item["groupId"].toString();

        state.groups_list.clear();

        QJsonObject json_groups = json_item["groups"].toObject();

        foreach(const QString& key, json_groups.keys()) {
            state.groups_list[key] = json_groups[key].toObject()["name"].toString();
        }
    }

    if (json.contains("execution")) {
        json_item = json["execution"].toObject();

        state.mode = json_item["mode"].toString();
        state.sync_active = json_item["syncActive"].toBool();
        state.hdmi_source = json_item["hdmiSource"].toString();
        state.last_sync_mode = json_item["lastSyncMode"].toString();
        state.brightness = json_item["brightness"].toInt();

        state.mode_list.clear();
        QMapIterator<QString, QString> m(mode_list);
        while (m.hasNext()) {
            m.next();

            state.mode_list[m.key()] = json_item[m.key()].toObject()["intensity"].toString();
        }
    }


    if (json.contains("hdmi")) {
        json_item = json["hdmi"].toObject();

        state.hdmies_name_list.clear();
        state.hdmies_type_list.clear();
        state.hdmies_status_list.clear();
        state.hdmies_sync_mode_list.clear();

        for (int i = 0; i < inputs.length(); ++i) {
            QJsonObject json_input;

            json_input = json_item[inputs[i]].toObject();

            state.hdmies_name_list[inputs[i]] = json_input["name"].toString();
            state.hdmies_type_list[inputs[i]] = json_input["type"].toString();
            state.hdmies_status_list[inputs[i]] = json_input["status"].toString();
            state.hdmies_sync_mode_list[inputs[i]] = json_input["lastSyncMode"].toString();
        }
    }

}

void SyncboxWidget::updateSyncbox(QJsonObject json)
{
    updateState(json);


    if (rebuild) {
        createModes();
        createIntensities();
        createHdmies();
        createGroups();
    }

    setMainBtn();
    setModes();
    setIntensities();
    setBrightness();
    setHdmies();
    setGroups();

    rebuild = false;
}