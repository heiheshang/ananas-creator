/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "ananasprojectmanager.h"
#include "ananasprojectconstants.h"
#include "ananasproject.h"

#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/messageoutputwindow.h>
#include <projectexplorer/projectexplorerconstants.h>

#include <QtDebug>

using namespace AnanasProjectManager::Internal;

Manager::Manager()
{
    Core::UniqueIDManager *uidm = Core::UniqueIDManager::instance();
    m_projectContext  = uidm->uniqueIdentifier(AnanasProjectManager::Constants::PROJECTCONTEXT);
    m_projectLanguage = uidm->uniqueIdentifier(AnanasProjectManager::Constants::LANG_QML);
}

Manager::~Manager()
{ }

int Manager::projectContext() const
{ return m_projectContext; }

int Manager::projectLanguage() const
{ return m_projectLanguage; }

QString Manager::mimeType() const
{ return QLatin1String(Constants::ANANASMIMETYPE); }

ProjectExplorer::Project *Manager::openProject(const QString &fileName)
{
    QFileInfo fileInfo(fileName);

    if (fileInfo.isFile()) {
        AnanasProject *project = new AnanasProject(this, fileName);
        return project;
    }

    return 0;
}

void Manager::registerProject(AnanasProject *project)
{ m_projects.append(project); }

void Manager::unregisterProject(AnanasProject *project)
{ m_projects.removeAll(project); }

void Manager::notifyChanged(const QString &fileName)
{
    foreach (AnanasProject *project, m_projects) {
        if (fileName == project->filesFileName()) {
            project->refresh(AnanasProject::Files);
        }
    }
}
