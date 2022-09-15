﻿#include "menu.h"
#include "global/logcategories.h"
#include "items/valueitem.h"

#include "global/logcategories.h"
#include "graphic/style.h"
#include "menubuilder.h"

Menu& Menu::getInstance()
{
    static Menu instance;
    return instance;
}

Menu::Menu() : QWidget()
{
    m_root = std::make_unique<SubmenuItem>(QString{}, nullptr, this);

    new QVBoxLayout(this);
    layout()->setAlignment(Qt::AlignTop);
    setStyleSheet(Style::getInstance().menu.main);
}

void Menu::keyPressEvent(QKeyEvent *event)
{
    if (m_active)
    {
        m_currentItem->control(event);
    }
    else if (event->key() == Qt::Key_Right)
    {
        open();
    }
}

void Menu::completeExec()
{
    if (m_currentSubmenu != nullptr)
    {
        setSubmenu(m_currentSubmenu);
    }
}

void Menu::setSubmenu(SubmenuItem *submenu)
{
    // TODO move partly to SubmenuItem

    if (submenu->itemList.size() == 0)
    {
        m_currentSubmenu->itemList[m_currentSubmenu->m_currentElement]->onSelect();
        m_currentSubmenu->itemList[m_currentSubmenu->m_currentElement]->setStyleSheet(Style::getInstance().menu.emptyItemList);
        qCWarning(uiLog()) << "Empty submenu";
        return;
    }


    if (m_currentSubmenu)
    {
        m_currentSubmenu->close();
    }

    // TODO add parent to currentItem so I ditch currentSubmenu
    m_currentItem = submenu;
    m_currentSubmenu = submenu;

    for (auto &&item : m_currentSubmenu->itemList)
    {
        if (!item->isHidden())
        {
            item->setVisible(true);
            layout()->addWidget(item.get());
        }
    }

    m_currentSubmenu->itemList[m_currentSubmenu->m_currentElement]->onSelect();
}

SubmenuItem* Menu::getRoot()
{
    return m_root.get();
}

void Menu::open()
{
    m_active = true;
    setSubmenu(m_root.get());
}

void Menu::close()
{
    m_active = false;

    for (auto &&item : m_currentSubmenu->itemList)
    {
        layout()->removeWidget(item.get());
        item->setVisible(false);
    }
}
