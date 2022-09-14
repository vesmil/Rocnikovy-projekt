#ifndef SUBMENUITEM_H
#define SUBMENUITEM_H

#include <vector>
#include <memory>
#include <qpushbutton.h>
#include <QLabel>

#include "itembase.h"

//! \brief Menu element from which next menu will be opened
class SubmenuItem : public ItemBase
{
public:
    SubmenuItem(QString text, SubmenuItem *parentMenu, QWidget *parent = nullptr);

    void executeSlected() override;
    void control(QKeyEvent* event) override;

    void close();

    bool isHidden() override;

    QLabel* m_text;
    size_t m_currentElement = 0;

    SubmenuItem* parentMenu;
    std::vector<std::unique_ptr<ItemBase>> itemList {};
};

#endif // SUBMENUITEM_H
