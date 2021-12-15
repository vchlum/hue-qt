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

#include <QPropertyAnimation>
#include <menucolorpicker.h>
#include "menuexpendable.h"

MenuExpendable::MenuExpendable(const int animation_duration, QWidget *parent) : QWidget(parent), animation_duration(animation_duration)
{
    header_line = new QFrame(this);
    toggle_animation = new QParallelAnimationGroup(this);
    content_area = new QScrollArea(this);
    main_layout = new QGridLayout(this);
    arrow_icon = new QLabel(this);

    content_area->setWidgetResizable(true);

    auto content_layout = new QVBoxLayout();
    setContentLayout(*content_layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    content_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // let the entire widget grow and shrink with its content
    toggle_animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggle_animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggle_animation->addAnimation(new QPropertyAnimation(content_area, "maximumHeight"));

    connect(toggle_animation, &QAbstractAnimation::finished, [this]() {
        adjustSize();
        emit menuToggled();
    });

    // start out collapsed
    content_area->setMaximumHeight(0);
    content_area->setMinimumHeight(0);
    is_collapsed = true;

    main_layout->setVerticalSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(main_layout);
}

void MenuExpendable::toggle(bool expanded) {
    if (is_collapsed != expanded) {
        return;
    }

    toggle_button->setChecked(expanded);
    is_collapsed = !expanded;
    setArrowType(!expanded ? ":images/arrowright.svg" : ":images/arrowdown.svg");
    toggle_animation->setDirection(expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggle_animation->start();
}

void MenuExpendable::setArrowType(QString arrow)
{
    QIcon icon (arrow);

    arrow_icon->setPixmap(icon.pixmap(QSize(15, 15)));
}

void MenuExpendable::setHeadMenuButton(MenuButton &button)
{

    header_line->setParent(NULL);
    content_area->setParent(NULL);
    arrow_icon->setParent(NULL);

    delete toggle_button;
    toggle_button = &button;

    toggle_button->layout()->addWidget(arrow_icon);
    toggle_button->layout()->setAlignment(arrow_icon, Qt::AlignRight);
    setArrowType(":images/arrowright.svg");

    toggle_button->setMinimumWidth(toggle_button->layout()->sizeHint().width());
    toggle_button->setMinimumHeight(toggle_button->layout()->sizeHint().height());
    toggle_button->adjustSize();

    toggle_button->setCheckable(true);
    toggle_button->setChecked(false);

    int row = 0;
    main_layout->addWidget(toggle_button, row, 0, 1, 1);
    main_layout->addWidget(header_line, row++, 2, 1, 1);
    main_layout->addWidget(content_area, row, 0, 1, 3);

    connect(toggle_button, &QPushButton::toggled, this, &MenuExpendable::toggle);

    if (!is_collapsed) {
        toggle(false);
    }
}

void MenuExpendable::adjustContentSize()
{
    auto content_widget = content_area->widget();

    if (content_widget->layout()->itemAt(0) == NULL) {
        return;
    }

    auto collapsedHeight = sizeHint().height() - content_area->maximumHeight();

    if (toggle_button) {
        collapsedHeight = toggle_button->sizeHint().height();
    }

    auto contentHeight = content_widget->sizeHint().height();

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
    content_area->setMinimumWidth(content_widget->sizeHint().width());

    adjustSize();
}

void MenuExpendable::setContentLayout(QLayout &content_layout)
{
    delete content_area->widget();
    QWidget *widget = new QWidget(content_area);
    widget->setLayout(&content_layout);
    content_area->setWidget(widget);

    adjustContentSize();
}

void MenuExpendable::setContentWidget(QWidget &content_widget)
{
    delete content_area->widget();
    content_area->setWidget(&content_widget);

    adjustContentSize();
}

void MenuExpendable::addContentMenuButton(MenuButton &button)
{
    auto content_layout = content_area->widget()->layout();
    content_layout->addWidget(&button);

    adjustContentSize();
}

void MenuExpendable::clearContentButtons()
{
    auto content_layout = content_area->widget()->layout();

    QLayoutItem* i;
    while ( (i = content_layout->takeAt(0)) != NULL ) {
        delete i->widget();
        delete i;
    }
}