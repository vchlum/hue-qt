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

#ifndef MAINMENUBRIDGE_H
#define MAINMENUBRIDGE_H

#include <QWidget>

#include <huebridge.h>
#include <menuexpendable.h>

class BridgeWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit BridgeWidget(HueBridge *showed_bridge, QWidget* parent = nullptr);

    protected:

    private:
        QString selected_group = "";
        QString selected_light = "";
        HueBridge *bridge;
        MenuExpendable* groups;
        MenuExpendable* lights;
        MenuExpendable* scenes;
        MenuExpendable* colors;
        QJsonObject bridge_data;
        bool rebuild = true;

        void setGroups(QString head_group_id);
        void setLights(QString group_id, QString head_light_id);
        void setScenes(QString group_id);
        void setColorsTemperature(QString group_id, QString light_id);

    private slots:
        void updateBridge(QJsonObject json);
        void autoResize();
        void groupClicked();
        void lightClicked();
        void sceneClicked();

    signals:
        void sizeChanged();
};

#endif // MAINMENUBRIDGE_H