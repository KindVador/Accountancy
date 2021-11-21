#include "contextualmenugenerator.hpp"
#include "../controller.hpp"

ContextualMenuGenerator *ContextualMenuGenerator::instance()
{
    static ContextualMenuGenerator instance;
    return &instance;
}

QMenu *ContextualMenuGenerator::ownerSectionMenu(const QObject *receiver)
{
    qDebug() << "ContextualMenuGenerator::ownerSectionMenu";
    auto *contextMenu = new QMenu;
    contextMenu->addAction("Reset selection", receiver, []() { Controller::getInstance()->clearOwnerSelection(); } );

    return contextMenu;
}