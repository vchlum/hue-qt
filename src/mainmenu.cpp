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

#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>
#include <QApplication>
#include <QInputDialog>

#include <menuswitch.h>

#include "mainmenu.h"
#include "menulayout.h"
#include "menubutton.h"

Menu::Menu(QWidget *parent): QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    bridge_list = new HueBridgeList();
    syncbox_list = new HueSyncboxList();

    bridge_list->loadBridges();
    syncbox_list->loadSyncboxes();

    connect(bridge_list, SIGNAL(bridgeDataUpdated()), this, SLOT(rebuildAll()));
    connect(syncbox_list, SIGNAL(syncboxDataUpdated()), this, SLOT(rebuildAll()));

    discovery = new HueBridgeDiscovery();
    connect(discovery, SIGNAL(bridgeDiscovered(QJsonObject, QString)), bridge_list, SLOT(createBridge(QJsonObject, QString)));
    connect(discovery, SIGNAL(bridgeDiscovered(QJsonObject, QString)), this, SLOT(updateSettingMenu()));
    discovery->discoverBridges();

    foreach(HueBridge *bridge, bridge_list->list) {
        if (!bridge->known()) {
            bridge->createUser();
            bridge_list->saveBridges();
        }

        if (bridge->known()) {
            bridge->getStatus();
            bridge->getConfig();
        }
    }

    foreach(HueSyncbox *syncbox, syncbox_list->list) {
        if (!syncbox->known()) {
            syncbox->createRegistration();
            syncbox_list->saveSyncboxes();
        }

        if (syncbox->known()) {
            syncbox->getDevice();
        }
    }

    rebuildAll();
}

void Menu::adjustWindow()
{
    adjustSize();
}

void Menu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void Menu::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}

void Menu::addDiscoveredBridge()
{
    QAction *act = qobject_cast<QAction *>(sender());
    QVariant id = act->data();

    HueBridge *bridge =  bridge_list->findBridge(id.toString());
    if (bridge != NULL && !bridge->known()) {
        bridge->createUser();
    }
}

void Menu::addBridgeIP()
{
    bool ok;
    QString ip_address = QInputDialog::getText(0, tr("Enter Philips Hue Bridge IP"),
        tr("Before confirmation, press the Philips Hue Bridge button."),
        QLineEdit::Normal,
        "", &ok);

    if (ok && !ip_address.isEmpty()) {
        HueBridge *bridge =  bridge_list->addBridge(ip_address);
        bridge->createUser();
    }
}

void Menu::addSyncboxIP()
{
    bool ok;
    QString ip_address = QInputDialog::getText(0, tr("Enter Philips Hue HDMI Syncbox IP"),
        tr("After confirmation, hold the Philips Hue HDMI Syncbox button for ~3 seconds (until green blink)."),
        QLineEdit::Normal,
        "", &ok);

    if (ok && !ip_address.isEmpty()) {
        HueSyncbox *syncbox =  syncbox_list->addSyncbox(ip_address);
        syncbox->createRegistration();
    }
}

void Menu::updateSettingMenu()
{
    delete button_settings->menu();

    QMenu *setting_menu = new QMenu();

    foreach(HueBridge *bridge, bridge_list->list) {
        if (!bridge->known()) {
            QAction *act_unknown_bridge = new QAction(tr("Add Philips Hue bridge") + " " + bridge->deviceName(), setting_menu);
            QVariant id (bridge->id());
            act_unknown_bridge->setData(id);

            connect(act_unknown_bridge, SIGNAL(triggered()), this, SLOT(addDiscoveredBridge()));
            setting_menu->addAction(act_unknown_bridge); 
        }
    }

    QAction *act_add_bridge_ip = new QAction(tr("Add Philips Hue Bridge with specific IP"), setting_menu);
    connect(act_add_bridge_ip, SIGNAL(triggered()), this, SLOT(addBridgeIP()));
    setting_menu->addAction(act_add_bridge_ip);

    QAction *act_add_syncbox_ip = new QAction(tr("Add Philips Hue HDMI Syncbox with specific IP"), setting_menu);
    connect(act_add_syncbox_ip, SIGNAL(triggered()), this, SLOT(addSyncboxIP()));
    setting_menu->addAction(act_add_syncbox_ip);

    QAction *act_rebuild = new QAction(tr("Refresh"), setting_menu);
    connect(act_rebuild, SIGNAL(triggered()), this, SLOT(rebuildAll()));
    setting_menu->addAction(act_rebuild);

    QAction *act_exit = new QAction(tr("Exit"), setting_menu);
    connect(act_exit, &QAction::triggered, this, &QApplication::quit);
    setting_menu->addAction(act_exit);    

    button_settings->setMenu(setting_menu);
}

void Menu::deviceButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());

    selected_device = btn->property("device_id").toString();

    rebuildAll();
}

QHBoxLayout* Menu::createDeviceMenu()
{
    QHBoxLayout *device_layout = new QHBoxLayout();
    device_layout->setAlignment(Qt::AlignRight);

    foreach(HueBridge *bridge, bridge_list->list) {
        if (bridge->known()) {
            QPushButton *bridge_button = new QPushButton();
            QIcon icon (":images/HueIcons/devicesBridgesV2.svg");
            QVariant id (bridge->id());
            if (id == "") {
                // We dont have the ID yet, use at least IP
                id = bridge->ip();
            }
            bridge_button->setProperty("device_id", id);
            bridge_button->setToolTip(bridge->deviceName());
            bridge_button->setIcon(icon);
            bridge_button->setIconSize(QSize(20, 20));
            bridge_button->setStyleSheet("QPushButton::menu-indicator {width:0px;} QPushButton {border: none;}");

            connect(bridge_button, SIGNAL(clicked()), this, SLOT(deviceButtonClicked()));

            device_layout->addWidget(bridge_button);
            device_layout->setAlignment(bridge_button, Qt::AlignLeft);
        }
    }

    foreach(HueSyncbox *syncbox, syncbox_list->list) {
        if (syncbox->known()) {
            QPushButton *syncbox_button = new QPushButton();
            QIcon icon (":images/syncbox.svg");
            QVariant id (syncbox->id());
            if (id == "") {
                // We dont have the ID yet, use at least IP
                id = syncbox->ip();
            }
            syncbox_button->setProperty("device_id", id);
            syncbox_button->setToolTip(syncbox->deviceName());
            syncbox_button->setIcon(icon);
            syncbox_button->setIconSize(QSize(20, 20));
            syncbox_button->setStyleSheet("QPushButton::menu-indicator {width:0px;} QPushButton {border: none;}");

            connect(syncbox_button, SIGNAL(clicked()), this, SLOT(deviceButtonClicked()));

            device_layout->addWidget(syncbox_button);
            device_layout->setAlignment(syncbox_button, Qt::AlignLeft);
        }
    }

    button_settings = new QPushButton();
    QIcon settings_icon (":images/HueIcons/tabbarSettings.svg");

    button_settings->setIcon(settings_icon);
    button_settings->setIconSize(QSize(20, 20));
    button_settings->setStyleSheet("QPushButton::menu-indicator {width:0px;} QPushButton {border: none;}");

    updateSettingMenu();

    device_layout->addWidget(button_settings);
    device_layout->setAlignment(button_settings, Qt::AlignRight);

    return device_layout;
}

void Menu::rebuildAll()
{
    if ( layout() != NULL ) {
        QLayoutItem* i;
        while ( ( i = layout()->takeAt( 0 ) ) != NULL ) {
            delete i->widget();
            delete i;
        }
        delete layout();
    }

    while ( QWidget* w = findChild<QWidget*>() )
        delete w;

    discovery->discoverBridges();

    QVBoxLayout *menu_layout = new QVBoxLayout(this);

    menu_layout->setAlignment(Qt::AlignTop);

    menu_layout->addLayout(createDeviceMenu());

    HueBridge *bridge = bridge_list->findBridge(selected_device);
    if (bridge != NULL) {
        if (bridge->known()) {
            bridge->getConfig();
        }
    }

    HueSyncbox *syncbox = syncbox_list->findSyncbox(selected_device);
    if (syncbox != NULL) {
        if (syncbox->known()) {
            syncbox->getDevice();
        }
    }

    auto foo = new MenuExpendable(300, 0);
    auto bar = new MenuLayout(true, true);
    bar->setIcon(":images/HueIcons/devicesBridgesV2.svg");
    bar->setText("foo bar");
    foo->setHeadLayout(*bar);

    auto *content_layout = new QVBoxLayout();

    auto itemlayout1 = new MenuLayout(false, true);
    itemlayout1->setText("foo");
    auto item1 = new MenuButton(itemlayout1);
    content_layout->addWidget(item1);

    auto itemlayout2 = new MenuLayout(true, false);
    itemlayout2->setIcon(":images/HueIcons/devicesBridgesV2.svg");
    itemlayout2->setText("bar");
    auto item2 = new MenuButton(itemlayout2);
    content_layout->addWidget(item2);

    auto itemlayout3 = new MenuLayout(false, false);
    itemlayout3->setText("test");
    auto item3 = new MenuButton(itemlayout3);
    content_layout->addWidget(item3);

    auto itemlayout4 = new MenuLayout(true, true);
    itemlayout4->setText("test2");
    auto item4 = new MenuButton(itemlayout4);
    itemlayout4->setColor("#FF0000");
    content_layout->addWidget(item4);

    foo->setContentLayout(*content_layout);

    connect(foo, SIGNAL(menuToggled()), this, SLOT(adjustWindow()));

    menu_layout->addWidget(foo);



    auto groups = new MenuExpendable(300, 0);
    auto main_group = new MenuLayout(true, true);
    main_group->setIcon(":images/HueIcons/devicesBridgesV2.svg");
    main_group->setText("Main group");
    groups->setHeadLayout(*main_group);

    auto *main_group_layout = new QVBoxLayout();

    auto itemlayout5 = new MenuLayout(true, true);
    itemlayout5->setText("test2");
    auto item5 = new MenuButton(itemlayout5);
    itemlayout4->setColor("#FF0000");
    main_group_layout->addWidget(item5);

    groups->setContentLayout(*main_group_layout);

    connect(groups, SIGNAL(menuToggled()), this, SLOT(adjustWindow()));

    menu_layout->addWidget(groups);












    adjustWindow();
}