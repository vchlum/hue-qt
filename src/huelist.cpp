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

#include "huelist.h"
#include <QtCore/qglobal.h>
#include <QDir>

HueList::HueList(QObject *parent): QObject(parent)
{

}

QString HueList::getStoragePath(QString name)
{
    #if defined(Q_OS_LINUX)
        QDir dir(QDir::homePath() + "/.config/hue-qt/");

        if (!dir.exists()) {
            dir.mkpath(".");
        }

        return qPrintable(dir.path() + "/" + name);
    #else
        return name;
    #endif
}
