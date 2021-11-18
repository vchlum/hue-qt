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

#include <QPixmap>
#include <QColor>
#include <QThread>
#include <QTimer>

#ifndef MENUUTILS_H
#define MENUUTILS_H

class TimerThread : public QThread
{
    Q_OBJECT
    void run() override {
        msleep(delay);
        emit timeIsUp();
    }

    private:
        int delay = 100;

    public:
        void setDelay(int d) {
            delay = d;
        }

    signals:
        void timeIsUp();
};

QPixmap getColorPixmapFromSVG(QString filename, QColor color);

#endif // MENUUTILS_H