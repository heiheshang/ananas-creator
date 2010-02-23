#ifndef ANANASVIEWNAVIGATIONWIDGETFACTORY_H
#define ANANASVIEWNAVIGATIONWIDGETFACTORY_H
#include <QObject>
#include <QTreeWidget>
#include "libananas/acfg.h"
#include <coreplugin/inavigationwidgetfactory.h>
namespace Core {
class INavigationWidgetFactory;
}


namespace AnanasProjectManager {
namespace Internal {

class AnanasViewNavigationWidgetFactory : public Core::INavigationWidgetFactory
{
public:
    AnanasViewNavigationWidgetFactory();
    ~AnanasViewNavigationWidgetFactory();

    Core::NavigationView createWidget();
    QString displayName();
};
}
}
#endif // ANANASVIEWNAVIGATIONWIDGETFACTORY_H
