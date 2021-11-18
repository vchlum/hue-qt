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

#ifndef MAINMENUBRIDGE_H
#define MAINMENUBRIDGE_H

#include <QWidget>

#include <huebridge.h>
#include <menuexpendable.h>

#include "mainmenubridgeutils.h"

class BridgeWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit BridgeWidget(HueBridge *showed_bridge, QWidget* parent = nullptr);

    protected:

    private:
        QString bridge_home_id = "";
        QString selected_group = "";
        QString selected_light = "";
        HueBridge *bridge;
        MenuExpendable* groups;
        MenuExpendable* lights;
        MenuExpendable* scenes;
        MenuExpendable* colors;
        bool rebuild = true;

        QMap<QString, ItemState> states_groups;
        QMap<QString, ItemState> states_lights;
        QMap<QString, ItemState> states_scenes;

        QVarLengthArray<QString> events_update_list;
        bool waiting_events = false;
        QMap<MenuButton*, QString> refresh_button_list;

        void addGroupState(QJsonObject json);
        void addLightState(QJsonObject json);
        void addSceneState(QJsonObject json);
        void createStates(QJsonObject json);
        QMap<QString, ItemState>* getStatesByType(QString type);

        void updateButtonState(MenuButton* button, ItemState state);
        MenuButton* createMenuButton(MenuExpendable* menu, ItemState state, void (BridgeWidget::*button_slot)() = NULL, bool back_button = false, QString custom_text = "");

        bool checkAnyServiceIsOn(QMapIterator<QString, QString> services, QString type);

        ItemState getCombinedGroupState(ItemState base_state);

        void setGroups();
        void setLights(QString group_id);
        void setScenes(QString group_id);
        void setColorsTemperature(QString light_id, QString group_id);

        void updateAllButtons();
        QString updateStateByEvent(QJsonObject json);

    private slots:
        void updateBridge(QJsonObject json);
        void updateRelatedButtons();
        void processEvents(QJsonArray &json_array);
        void autoResize();
        void groupClicked();
        void lightClicked();
        void sceneClicked();
        void removeFromButtonList(QString main_id);

        void switchId(QString id, bool on);
        void dimmId(QString id, int value);

    signals:
        void sizeChanged();
};

#endif // MAINMENUBRIDGE_H