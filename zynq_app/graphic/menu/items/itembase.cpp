#include "itembase.h"

#include <QPalette>
#include <graphic/style.h>
#include "global/logcategories.h"

ItemBase::ItemBase(QWidget* parent, SubmenuItem* parentMenu) : QWidget(parent), m_parentMenu(parentMenu)
{
    setVisible(false);
    setStyleSheet(Style::getInstance().menu.item);
}

void ItemBase::execute()
{    
    setStyleSheet(Style::getInstance().menu.executedItem);
}

void ItemBase::onSelect()
{
    setStyleSheet(Style::getInstance().menu.selectedItem);
}

void ItemBase::onDeselect()
{
    setStyleSheet(Style::getInstance().menu.item);
}

void ItemBase::control(QKeyEvent*)
{
    qCWarning(gsLog()) << "Calling control on abstract type";
}

void ItemBase::refresh()
{

}

bool ItemBase::isHidden()
{
    return false;
}
