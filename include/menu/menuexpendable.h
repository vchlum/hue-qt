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

#ifndef MENUEXPENDABLE_H
#define MENUEXPENDABLE_H

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QStyle>

#include "menulayout.h"

// https://stackoverflow.com/questions/32476006/how-to-make-an-expandable-collapsable-section-widget-in-qt
// https://github.com/MichaelVoelkel/qt-collapsible-section

class MenuExpendable : public QWidget
{
    Q_OBJECT
    private:
        QGridLayout* main_layout;
        QPushButton* toggle_button;
        QFrame* header_line;
        QParallelAnimationGroup* toggle_animation;
        QScrollArea* content_area;
        int animation_duration;
        QLabel *arrow_icon;

        void setArrowType(QString arrow);

    public slots:
        void toggle(bool collapsed);

    signals:
        void menuToggled();

    public:
        explicit MenuExpendable(const int animation_duration = 100, QWidget* parent = 0);
        void setHeadLayout(QHBoxLayout &content_layout);
        void setContentLayout(QLayout &content_layout);
};

#endif // MENUEXPENDABLE_H