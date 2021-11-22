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

#ifndef MAINMENUSYNCBOX_H
#define MAINMENUSYNCBOX_H

#include <QWidget>

#include <huesyncbox.h>
#include <menuexpendable.h>
#include <menubutton.h>

#include "mainmenusyncboxutils.h"

class SyncboxWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit SyncboxWidget(HueSyncbox *showed_syncbox, QWidget* parent = nullptr);

    protected:

    private:
        HueSyncbox *syncbox;
        SyncboxState state;
        bool rebuild = true;

        MenuButton* main_btn;
        MenuButton* mode_btn;
        MenuButton* intens_btn;
        MenuButton* brightness_btn;
        MenuButton* hdmi_btn;
        MenuButton* group_btn;

        MenuExpendable* modes;
        MenuExpendable* intensities;
        MenuExpendable* hdmies;
        MenuExpendable* groups;

        void createModes();
        void createIntensities();
        void createHdmies();
        void createGroups();

        void setMainBtn();
        void setModes();
        void setIntensities();
        void setBrightness();
        void setHdmies();
        void setGroups();

    private slots:
        void checkSyncbox();
        void updateState(QJsonObject json);
        void updateSyncbox(QJsonObject json);
        void autoResize();
        void changePower(QString id, bool on);
        void changeBrightness(QString id, int value);
        void changeSync(QString id, bool on);
        void changeMode();
        void changeIntensity();
        void changeInput();
        void changeGroup();

    signals:
        void sizeChanged();
};

#endif // MAINMENUSYNCBOX_H