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

#include <QPropertyAnimation>

#include "menuexpendable.h"

MenuExpendable::MenuExpendable(const int animation_duration, QWidget *parent) : QWidget(parent), animation_duration(animation_duration)
{
    header_line = new QFrame(this);
    toggle_animation = new QParallelAnimationGroup(this);
    content_area = new QScrollArea(this);
    main_layout = new QGridLayout(this);
    arrow_icon = new QLabel(this);

    content_area->setWidgetResizable( true );

    auto content_layout = new QVBoxLayout();
    setContentLayout(*content_layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}

void MenuExpendable::toggle(bool collapsed) {
    MenuExpendable::setArrowType(collapsed ? ":images/arrowdown.svg" : ":images/arrowright.svg");
    toggle_animation->setDirection(collapsed ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggle_animation->start();

    auto content_layout = content_area->widget()->layout();
    auto contentHeight = content_layout->sizeHint().height();
}

void MenuExpendable::setArrowType(QString arrow)
{
    QIcon icon (arrow);

    arrow_icon->setPixmap(icon.pixmap(QSize(15, 15)));
}

void MenuExpendable::setHeadMenuButton(MenuButton &button)
{
    delete toggle_button;

    toggle_button = &button;

    toggle_button->layout()->addWidget(arrow_icon);
    toggle_button->layout()->setAlignment(arrow_icon, Qt::AlignRight);

    toggle_button->setMinimumWidth(toggle_button->layout()->sizeHint().width());
    toggle_button->setMinimumHeight(toggle_button->layout()->sizeHint().height());
    toggle_button->adjustSize();
    adjustSize();

    toggle(false);

    toggle_button->setCheckable(true);
    toggle_button->setChecked(false);

    //header_line->setFrameShape(QFrame::HLine);
    //header_line->setFrameShadow(QFrame::Sunken);
    //header_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    content_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // start out collapsed
    content_area->setMaximumHeight(0);
    content_area->setMinimumHeight(0);

    // let the entire widget grow and shrink with its content
    toggle_animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggle_animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggle_animation->addAnimation(new QPropertyAnimation(content_area, "maximumHeight"));

    main_layout->setVerticalSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);

    int row = 0;
    main_layout->addWidget(toggle_button, row, 0, 1, 1);
    main_layout->addWidget(header_line, row++, 2, 1, 1);
    main_layout->addWidget(content_area, row, 0, 1, 3);
    setLayout(main_layout);

    connect(toggle_button, &QPushButton::toggled, this, &MenuExpendable::toggle);
    connect(toggle_animation, &QAbstractAnimation::finished, [this]() {
        adjustSize();
        emit menuToggled();
    });
}

void MenuExpendable::adjustContentSize()
{
    auto content_layout = content_area->widget()->layout();

    if (content_layout->itemAt(0) == NULL) {
        return;
    }

    const auto collapsedHeight = sizeHint().height() - content_area->maximumHeight();
    auto contentHeight = content_layout->sizeHint().height();

    contentHeight = contentHeight > 500 ? 500 : contentHeight;

    for (int i = 0; i < toggle_animation->animationCount() - 1; ++i)
    {
        QPropertyAnimation* section_animation = static_cast<QPropertyAnimation *>(toggle_animation->animationAt(i));
        section_animation->setDuration(animation_duration);
        section_animation->setStartValue(collapsedHeight);
        section_animation->setEndValue(collapsedHeight + contentHeight);
    }

    QPropertyAnimation* content_animation = static_cast<QPropertyAnimation *>(toggle_animation->animationAt(toggle_animation->animationCount() - 1));
    content_animation->setDuration(animation_duration);
    content_animation->setStartValue(0);
    content_animation->setEndValue(contentHeight);
    content_area->setMinimumWidth(content_layout->sizeHint().width());

    adjustSize();
}

void MenuExpendable::setContentLayout(QLayout &content_layout)
{
    delete content_area->widget();
    QWidget *widget = new QWidget();
    widget->setLayout( &content_layout );
    content_area->setWidget(widget);

    adjustContentSize();
}

void MenuExpendable::addContentMenuButton(MenuButton &button)
{
    auto content_layout = content_area->widget()->layout();
    content_layout->addWidget(&button);

    adjustContentSize();
}