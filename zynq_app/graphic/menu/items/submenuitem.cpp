#include "submenuitem.h"

#include <QHBoxLayout>
#include "../menu.h"

#include "global/logcategories.h"
#include <graphic/menu/itemlayout.h>

SubmenuItem::SubmenuItem(QString text, SubmenuItem *parentMenu, QWidget *parent)
    : ItemBase(parent), parentMenu(parentMenu)
{
    ItemLayout* layout = new ItemLayout(this);

    layout->setMargin(0);
    layout->setSpacing(0);

    m_text = new QLabel();
    m_text->setText(text);
    m_text->setVisible(true);

    layout->addWidget(m_text);
}

void SubmenuItem::execute()
{
    Menu::getInstance().setSubmenu(this);
}
