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

#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QEnterEvent>

#include "menuswitch.h"

MenuSwitch::MenuSwitch(QWidget *parent) : QAbstractButton(parent)
{
    animation = new QPropertyAnimation(this, "offset", this);
    setOffset(tmp_height / 2);
    y = tmp_height / 2;
    brush = QBrush((QColor("#2E2E2E")));
}

MenuSwitch::MenuSwitch(const QBrush &brsh, QWidget *parent) : QAbstractButton(parent)
{
    animation = new QPropertyAnimation(this, "offset", this);
    setOffset(tmp_height / 2);
    y = tmp_height / 2;
    brush = brsh;
}

void MenuSwitch::paintEvent(QPaintEvent *e) {
    (void) e;

    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (isEnabled()) {
        p.setBrush(state ? brush : Qt::black);
        p.setOpacity(state ? 0.5 : 0.38);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(QRect(margin, margin, width() - 2 * margin, height() - 2 * margin), 5.0, 5.0);
        p.setBrush(state ? brush : thumb);
        p.setOpacity(1.0);
        //p.drawEllipse(QRectF(offset() - (tmp_height / 2), y - (tmp_height / 2), height(), height()));
        p.drawRoundedRect(QRect(offset() - (tmp_height / 2), y - (tmp_height / 2), height(), height()), 5.0, 5.0);
    } else {
        p.setBrush(Qt::black);
        p.setOpacity(0.12);
        p.drawRoundedRect(QRect(margin, margin, width() - 2 * margin, height() - 2 * margin), 8.0, 8.0);
        p.setOpacity(1.0);
        p.setBrush(QColor("#BDBDBD"));
        p.drawEllipse(QRectF(offset() - (tmp_height / 2), y - (tmp_height / 2), height(), height()));
    }
}

void MenuSwitch::doAnimatedSwitch()
{
    if (state) {
        animation->setStartValue(tmp_height / 2);
        animation->setEndValue(width() - tmp_height);
        animation->setDuration(120);
        animation->start();
    } else {
        animation->setStartValue(offset());
        animation->setEndValue(tmp_height / 2);
        animation->setDuration(120);
        animation->start();
    }
}

void MenuSwitch::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() & Qt::LeftButton) {
        state = state ? false : true;
        thumb = state ? brush : QBrush("#d5d5d5");
        doAnimatedSwitch();
    }
    QAbstractButton::mouseReleaseEvent(e);
}

void MenuSwitch::enterEvent(QEnterEvent *e) {
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(e);
}

QSize MenuSwitch::sizeHint() const {
    return QSize(2 * (tmp_height + margin), tmp_height + 2 * margin);
}

void MenuSwitch::setColor(QColor color)
{
    brush = QBrush(color);
    repaint();
}

void MenuSwitch::setValue(bool on){
    if (on != state) {
        state = on;

        if (!state) {
            thumb = QBrush("#d5d5d5");
            repaint();
        }

        doAnimatedSwitch();
    }
}

bool MenuSwitch::value(){
    return state;
}