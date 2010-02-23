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

#ifndef ANANAsPROJECT_H
#define ANANASPROJECT_H

#include "ananasprojectmanager.h"
#include "ananasprojectnodes.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/buildstep.h>
#include <projectexplorer/applicationrunconfiguration.h>
#include <coreplugin/ifile.h>

#include <extensionsystem/iplugin.h>
#include <coreplugin/editormanager/ieditorfactory.h>
#include <texteditor/basetexteditor.h>

#include <QtCore/QDir>

namespace AnanasProjectManager {
namespace Internal {

class AnanasMakeStep;
class AnanasProjectFile;

class AnanasProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    AnanasProject(Manager *manager, const QString &filename);
    virtual ~AnanasProject();

    QString filesFileName() const;

    virtual QString name() const;
    virtual Core::IFile *file() const;
    virtual Manager *projectManager() const;
    virtual ProjectExplorer::IBuildConfigurationFactory *buildConfigurationFactory() const;

    virtual QList<ProjectExplorer::Project *> dependsOn();

    virtual bool isApplication() const;
    virtual bool hasBuildSettings() const;

    virtual ProjectExplorer::Environment environment(ProjectExplorer::BuildConfiguration *configuration) const;
    virtual QString buildDirectory(ProjectExplorer::BuildConfiguration *configuration) const;

    virtual ProjectExplorer::BuildConfigWidget *createConfigWidget();
    virtual QList<ProjectExplorer::BuildConfigWidget*> subConfigWidgets();

    virtual void newBuildConfiguration(const QString &buildConfiguration);
    virtual AnanasProjectNode *rootProjectNode() const;
    virtual QStringList files(FilesMode fileMode) const;

    QStringList targets() const;
    AnanasMakeStep *makeStep() const;
    QString buildParser(const QString &buildConfiguration) const;

    enum RefreshOptions {
        Files         = 0x01,
        Configuration = 0x02,
        Everything    = Files | Configuration
    };

    void refresh(RefreshOptions options);

    QDir projectDir() const;
    QStringList files() const;

protected:
    virtual void saveSettingsImpl(ProjectExplorer::PersistentSettingsWriter &writer);
    virtual bool restoreSettingsImpl(ProjectExplorer::PersistentSettingsReader &reader);

private:
    void parseProject(RefreshOptions options);
    QStringList convertToAbsoluteFiles(const QStringList &paths) const;

    Manager *m_manager;
    QString m_fileName;
    QString m_filesFileName;
    AnanasProjectFile *m_file;
    QString m_projectName;

    QStringList m_files;

    AnanasProjectNode *m_rootNode;
};

class AnanasProjectFile : public Core::IFile
{
    Q_OBJECT

public:
    AnanasProjectFile(AnanasProject *parent, QString fileName);
    virtual ~AnanasProjectFile();

    virtual bool save(const QString &fileName = QString());
    virtual QString fileName() const;

    virtual QString defaultPath() const;
    virtual QString suggestedFileName() const;
    virtual QString mimeType() const;

    virtual bool isModified() const;
    virtual bool isReadOnly() const;
    virtual bool isSaveAsAllowed() const;

    virtual void modified(ReloadBehavior *behavior);

private:
    AnanasProject *m_project;
    QString m_fileName;
};

class AnanasRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
    Q_OBJECT
public:
    AnanasRunConfiguration(AnanasProject *pro);
    virtual ~AnanasRunConfiguration();

    virtual QString type() const;
    virtual QString executable() const;
    virtual RunMode runMode() const;
    virtual QString workingDirectory() const;
    virtual QStringList commandLineArguments() const;
    virtual ProjectExplorer::Environment environment() const;
    virtual QString dumperLibrary() const;
    virtual QStringList dumperLibraryLocations() const;
    virtual QWidget *configurationWidget();

    ProjectExplorer::ToolChain::ToolChainType toolChainType() const { return ProjectExplorer::ToolChain::OTHER; }

    virtual void save(ProjectExplorer::PersistentSettingsWriter &writer) const;
    virtual void restore(const ProjectExplorer::PersistentSettingsReader &reader);

private Q_SLOTS:
    void setMainScript(const QString &scriptFile);
    void onQmlViewerChanged();


private:
    QString mainScript() const;

private:
    AnanasProject *m_project;
    QString m_scriptFile;
    QString m_qmlViewer;
    QString m_qmlViewerArgs;
    QLatin1String m_type;
};

class AnanasRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
    Q_OBJECT

public:
    AnanasRunConfigurationFactory();
    virtual ~AnanasRunConfigurationFactory();

    // used to recreate the runConfigurations when restoring settings
    virtual bool canRestore(const QString &type) const;

    // used to show the list of possible additons to a project, returns a list of types
    virtual QStringList availableCreationTypes(ProjectExplorer::Project *pro) const;

    // used to translate the types to names to display to the user
    virtual QString displayNameForType(const QString &type) const;

    virtual QSharedPointer<ProjectExplorer::RunConfiguration> create(ProjectExplorer::Project *project,
                                                                     const QString &type);

private:
    QLatin1String m_type;
};


} // namespace Internal
} // namespace AnanasProjectManager

#endif // ANANASPROJECT_H
