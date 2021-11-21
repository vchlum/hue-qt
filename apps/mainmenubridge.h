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
#include <QMap>

#include <huebridge.h>
#include <menuexpendable.h>

#include "mainmenubridgeutils.h"

const QMap<QString, QString> icon_list{
// rooms
{"living_room", ":images/HueIcons/roomsLiving.svg"},
{"kitchen", ":images/HueIcons/roomsKitchen.svg"},
{"dining", ":images/HueIcons/roomsDining.svg"},
{"bedroom", ":images/HueIcons/roomsBedroom.svg"},
{"kids_bedroom", ":images/HueIcons/roomsKidsbedroom.svg"},
{"bathroom", ":images/HueIcons/roomsBathroom.svg"},
{"nursery", ":images/HueIcons/roomsNursery.svg"},
{"recreation", ":images/HueIcons/roomsRecreation.svg"},
{"office", ":images/HueIcons/roomsOffice.svg"},
{"gym", ":images/HueIcons/roomsGym.svg"},
{"hallway", ":images/HueIcons/roomsHallway.svg"},
{"toilet", ":images/HueIcons/roomsToilet.svg"},
{"front_door", ":images/HueIcons/roomsFrontdoor.svg"},
{"garage", ":images/HueIcons/roomsGarage.svg"},
{"terrace", ":images/HueIcons/roomsTerrace.svg"},
{"garden", ":images/HueIcons/roomsOutdoorSocialtime.svg"},
{"driveway", ":images/HueIcons/roomsDriveway.svg"},
{"carport", ":images/HueIcons/roomsCarport.svg"},
{"home", ":images/HueIcons/tabbarHome.svg"},
{"downstairs", ":images/HueIcons/zonesAreasGroundfloor.svg"},
{"upstairs", ":images/HueIcons/zonesAreasFirstfloor.svg"},
{"top_floor", ":images/HueIcons/zonesAreasSecondfloor.svg"},
{"attic", ":images/HueIcons/roomsAttic.svg"},
{"guest_room", ":images/HueIcons/roomsGuestroom.svg"},
{"staircase", ":images/HueIcons/roomsStaircase.svg"},
{"lounge", ":images/HueIcons/roomsLounge.svg"},
{"man_cave", ":images/HueIcons/roomsMancave.svg"},
{"computer", ":images/HueIcons/roomsComputer.svg"},
{"studio", ":images/HueIcons/roomsStudio.svg"},
{"music", ":images/HueIcons/otherMusic.svg"},
{"tv", ":images/HueIcons/otherWatchingMovie.svg"},
{"reading", ":images/HueIcons/otherReading.svg"},
{"closet", ":images/HueIcons/roomsCloset.svg"},
{"storage", ":images/HueIcons/roomsStorage.svg"},
{"laundry_room", ":images/HueIcons/roomsLaundryroom.svg"},
{"balcony", ":images/HueIcons/roomsBalcony.svg"},
{"porch", ":images/HueIcons/roomsPorch.svg"},
{"barbecue", ":images/HueIcons/roomsSocialtime.svg"},
{"pool", ":images/HueIcons/roomsPool.svg"},
{"other", ":images/HueIcons/roomsOther.svg"},

//lights
{"classic_bulb", ":images/HueIcons/bulbsClassic.svg"},
{"sultan_bulb", ":images/HueIcons/bulbsSultan.svg"},
{"flood_bulb", ":images/HueIcons/bulbFlood.svg"},
{"spot_bulb", ":images/HueIcons/bulbsSpot.svg"},
{"candle_bulb", ":images/HueIcons/bulbCandle.svg"},
{"luster_bulb", ":images/HueIcons/bulbsClassic.svg"}, /* not sure*/
{"pendant_round", ":images/HueIcons/archetypesPendantRound.svg"},
{"pendant_long", ":images/HueIcons/archetypesPendantLong.svg"},
{"ceiling_round", ":images/HueIcons/archetypesCeilingRound.svg"},
{"ceiling_square", ":images/HueIcons/archetypesCeilingSquare.svg"},
{"floor_shade", ":images/HueIcons/archetypesFloorShade.svg"},
{"floor_lantern", ":images/HueIcons/archetypesFloorLantern.svg"},
{"table_shade", ":images/HueIcons/archetypesTableShade.svg"},
{"recessed_ceiling", ":images/HueIcons/archetypesRecessedCeiling.svg"},
{"recessed_floor", ":images/HueIcons/archetypesRecessedFloor.svg"},
{"single_spot", ":images/HueIcons/archetypesSingleSpot.svg"},
{"double_spot", ":images/HueIcons/archetypesDoubleSpot.svg"},
{"table_wash", ":images/HueIcons/archetypesTableWash.svg"},
{"wall_lantern", ":images/HueIcons/archetypesWallLantern.svg"},
{"wall_shade", ":images/HueIcons/archetypesWallShade.svg"},
{"flexible_lamp", ":images/HueIcons/archetypesDeskLamp.svg"}, /* not sure */
{"ground_spot", ":images/HueIcons/archetypesFloorSpot.svg"}, /* not sure */
{"wall_spot", ":images/HueIcons/archetypesWallSpot.svg"},
{"plug", ":images/HueIcons/devicesPlug.svg"},
{"hue_go", ":images/HueIcons/heroesHuego.svg"},
{"hue_lightstrip", ":images/HueIcons/heroesLightstrip.svg"},
{"hue_iris", ":images/HueIcons/heroesIris.svg"},
{"hue_bloom", ":images/HueIcons/heroesBloom.svg"},
{"bollard", ":images/HueIcons/archetypesBollard.svg"},
{"wall_washer", ":images/HueIcons/heroesHueplay.svg"}, /* not sure */
{"hue_play", ":images/HueIcons/heroesHueplay.svg"},
{"vintage_bulb", ":images/HueIcons/bulbsClassic.svg"}, /* not sure */
{"christmas_tree", ":images/HueIcons/otherChristmasTree.svg"},
{"hue_centris", ":images/HueIcons/archetypesCeilingSquare.svg"}, /* not sure */
{"hue_lightstrip_tv", ":images/HueIcons/heroesLightstrip.svg"}, /* not sure */
{"hue_tube", ":images/HueIcons/heroesLightstrip.svg"}, /* not sure */
{"hue_signe", ":images/HueIcons/heroesLightstrip.svg"} /* not sure */
};

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
        MenuButton* createMenuButton(
            MenuExpendable* menu,
            ItemState state,
            void (BridgeWidget::*button_slot)() = NULL,
            bool back_button = false,
            QString custom_text = "",
            QString custom_icon = "",
            bool combined = false,
            bool combined_all = false
        );

        bool checkAnyServiceIsOn(QMapIterator<QString, QString> services, QString type);
        bool checkAllServicesAreOn(QMapIterator<QString, QString> services, QString type);

        ItemState getCombinedGroupState(ItemState base_state);

        void setGroups();
        void setLights(QString group_id);
        void setScenes(QString group_id);
        void setColorsTemperature(QString light_id, QString group_id, int gradient_point = -1);

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
        void changeColorGradient(QString id, QColor color);
        void changeColor(QString id, QColor color);
        void changeMirek(QString id, int mirek);
        void gradientSelected(QString id, int point);

    signals:
        void sizeChanged();
};

#endif // MAINMENUBRIDGE_H