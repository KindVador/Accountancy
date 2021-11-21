#ifndef ACCOUNTANCY_CONTEXTUALMENUGENERATOR_HPP
#define ACCOUNTANCY_CONTEXTUALMENUGENERATOR_HPP

#include <QMenu>

class ContextualMenuGenerator
{
public:
    static ContextualMenuGenerator *instance();
    static QMenu *ownerSectionMenu(const QObject *receiver);

private:
    QList<QAction *> _actions;

    ContextualMenuGenerator() = default;
};


#endif //ACCOUNTANCY_CONTEXTUALMENUGENERATOR_HPP
