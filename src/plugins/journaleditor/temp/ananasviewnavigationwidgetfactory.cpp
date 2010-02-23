#include <coreplugin/filemanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/ifile.h>

#include "ananasviewnavigationwidgetfactory.h"
#include "ananasexplorersidebar.h"
#include <projectexplorer/projectexplorer.h>
#include <qdebug.h>
using namespace AnanasProjectManager;
using namespace AnanasProjectManager::Internal;

AnanasViewNavigationWidgetFactory::AnanasViewNavigationWidgetFactory()
{

}

AnanasViewNavigationWidgetFactory::~AnanasViewNavigationWidgetFactory()
{
}
Core::NavigationView AnanasViewNavigationWidgetFactory::createWidget()
{
    Core::NavigationView view;
    view.widget = new AnanasExplorerSideBar(0);

    return view;
}

QString AnanasViewNavigationWidgetFactory::displayName()
{
    return "Ananas";
}

