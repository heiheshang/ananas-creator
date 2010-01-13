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

#include "ananasmakestep.h"
#include "ananasprojectconstants.h"
#include "ananasproject.h"

#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>

using namespace AnanasProjectManager;
using namespace AnanasProjectManager::Internal;

AnanasMakeStep::AnanasMakeStep(AnanasProject *pro)
    : AbstractMakeStep(pro), m_pro(pro)
{
}

AnanasMakeStep::~AnanasMakeStep()
{
}

bool AnanasMakeStep::init(const QString &buildConfiguration)
{
    return AbstractMakeStep::init(buildConfiguration);
}

void AnanasMakeStep::run(QFutureInterface<bool> &fi)
{
    m_futureInterface = &fi;
    m_futureInterface->setProgressRange(0, 100);
    AbstractMakeStep::run(fi);
    m_futureInterface->setProgressValue(100);
    m_futureInterface = 0;
}

QString AnanasMakeStep::name()
{
    return Constants::MAKESTEP;
}

QString AnanasMakeStep::displayName()
{
    return QLatin1String("Ananas Make");
}

ProjectExplorer::BuildStepConfigWidget *AnanasMakeStep::createConfigWidget()
{
    return new AnanasMakeStepConfigWidget(this);
}

bool AnanasMakeStep::immutable() const
{
    return true;
}

void AnanasMakeStep::stdOut(const QString &line)
{
    AbstractMakeStep::stdOut(line);
}

AnanasProject *AnanasMakeStep::project() const
{
    return m_pro;
}

bool AnanasMakeStep::buildsTarget(const QString &, const QString &) const
{
    return true;
}

void AnanasMakeStep::setBuildTarget(const QString &, const QString &, bool)
{
}

QStringList AnanasMakeStep::additionalArguments(const QString &) const
{
    return QStringList();
}

void AnanasMakeStep::setAdditionalArguments(const QString &, const QStringList &)
{
}

//
// AnanasMakeStepConfigWidget
//

AnanasMakeStepConfigWidget::AnanasMakeStepConfigWidget(AnanasMakeStep *makeStep)
    : m_makeStep(makeStep)
{
}

QString AnanasMakeStepConfigWidget::displayName() const
{
    return QLatin1String("Ananas Make");
}

void AnanasMakeStepConfigWidget::init(const QString &)
{
}

QString AnanasMakeStepConfigWidget::summaryText() const
{
    return tr("<b>Ananas Make</b>");
}

//
// AnanasMakeStepFactory
//

bool AnanasMakeStepFactory::canCreate(const QString &name) const
{
    return (Constants::MAKESTEP == name);
}

ProjectExplorer::BuildStep *AnanasMakeStepFactory::create(ProjectExplorer::Project *project, const QString &name) const
{
    Q_ASSERT(name == Constants::MAKESTEP);
    AnanasProject *pro = qobject_cast<AnanasProject *>(project);
    Q_ASSERT(pro);
    return new AnanasMakeStep(pro);
}

QStringList AnanasMakeStepFactory::canCreateForProject(ProjectExplorer::Project *) const
{
    return QStringList();
}

QString AnanasMakeStepFactory::displayNameForName(const QString &) const
{
    return QLatin1String("Ananas Make");
}

