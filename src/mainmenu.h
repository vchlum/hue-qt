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

#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QMouseEvent>

#include <menuexpendable.h>

#include <huebridgelist.h>
#include <huebridge.h>
#include <huesyncboxlist.h>
#include <huesyncbox.h>

#include "mainmenubridge.h"

class Menu : public QWidget
{
    Q_OBJECT
    public:
        explicit Menu(QWidget *parent = 0);

    private:
        QPoint dragPosition;

        int counter;
        QMenuBar *menu_bar;
        QLabel *device_label;

        HueBridgeList *bridge_list;
        HueBridgeDiscovery *discovery;
        HueSyncboxList *syncbox_list;

        QPushButton *button_settings;
        QString selected_device = "";

        QWidget* createDeviceMenu();

    protected:
        void mouseMoveEvent(QMouseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;

    signals:

    private slots:
        void adjustWindow();
        void updateSettingMenu();
        void addDiscoveredBridge();
        void addBridgeIP();
        void addSyncboxIP();
        void rebuildAll();
        void deviceButtonClicked();
};

#endif // MAINMENU_H