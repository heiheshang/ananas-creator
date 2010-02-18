
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

#ifndef QMLMAKESTEP_H
#define QMLMAKESTEP_H

#include <projectexplorer/abstractmakestep.h>

namespace AnanasProjectManager {
namespace Internal {

class AnanasProject;

class AnanasMakeStep : public ProjectExplorer::AbstractMakeStep
{
    Q_OBJECT

public:
    AnanasMakeStep(AnanasProject *pro);
    virtual ~AnanasMakeStep();

    virtual bool init(const QString &buildConfiguration);
    virtual void run(QFutureInterface<bool> &fi);

    virtual QString name();
    virtual QString displayName();
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget();
    virtual bool immutable() const;

    AnanasProject *project() const;

    bool buildsTarget(const QString &buildConfiguration, const QString &target) const;
    void setBuildTarget(const QString &buildConfiguration, const QString &target, bool on);

    QStringList additionalArguments(const QString &buildConfiguration) const;
    void setAdditionalArguments(const QString &buildConfiguration, const QStringList &list);

protected:
    virtual void stdOut(const QString &line);

private:
    AnanasProject *m_pro;
    QFutureInterface<bool> *m_futureInterface;
};

class AnanasMakeStepConfigWidget :public ProjectExplorer::BuildStepConfigWidget
{
    Q_OBJECT

public:
    AnanasMakeStepConfigWidget(AnanasMakeStep *makeStep);

    virtual QString displayName() const;
    virtual void init(const QString &buildConfiguration);
    virtual QString summaryText() const;

private:
    AnanasMakeStep *m_makeStep;
};

class AnanasMakeStepFactory : public ProjectExplorer::IBuildStepFactory
{
public:
    virtual bool canCreate(const QString &name) const;
    virtual ProjectExplorer::BuildStep *create(ProjectExplorer::Project *pro, const QString &name) const;
    virtual QStringList canCreateForProject(ProjectExplorer::Project *pro) const;
    virtual QString displayNameForName(const QString &name) const;
};

} // namespace Internal
} // namespace AnanasProjectManager

#endif // MAKESTEP_H
