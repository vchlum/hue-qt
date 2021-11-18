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

#ifndef MENUSWITCH_H
#define MENUSWITCH_H

#include <QAbstractButton>
#include <QPropertyAnimation>

// https://stackoverflow.com/questions/14780517/toggle-switch-in-qt

class MenuSwitch : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

    public:
        MenuSwitch(QWidget* parent = nullptr);
        MenuSwitch(const QBrush& brush, QWidget* parent = nullptr);
        void setColor(QColor color);

        QSize sizeHint() const override;

        QBrush brush() const {
            return _brush;
        }
        void setBrush(const QBrush &brsh) {
            _brush = brsh;
        }

        int offset() const {
            return _x;
        }
        void setOffset(int o) {
            _x = o;
            update();
        }

        void setValue(bool on);
        bool value();

    protected:
        void paintEvent(QPaintEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void enterEvent(QEnterEvent*);

    private:
        bool _switch;
        qreal _opacity;
        int _x, _y, _height, _margin;
        QBrush _thumb, _track, _brush;
        QPropertyAnimation *_anim = nullptr;
        void doAnimatedSwitch();
};

#endif // MENUSWITCH_H