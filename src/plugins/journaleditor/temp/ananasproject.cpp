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

#include "ananasproject.h"
#include "ananasprojectconstants.h"
#include "ananasmakestep.h"

#include <projectexplorer/toolchain.h>
#include <projectexplorer/persistentsettings.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/pathchooser.h>
#include <utils/qtcassert.h>
#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>

#include <utils/synchronousprocess.h>
#include <utils/pathchooser.h>

#include <QtCore/QtDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>

#include <QtGui/QFormLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QComboBox>
#include <QtGui/QMessageBox>
#include <QtGui/QLineEdit>

using namespace AnanasProjectManager;
using namespace AnanasProjectManager::Internal;
using namespace ProjectExplorer;
////////////////////////////////////////////////////////////////////////////////////
// AnanasProject
////////////////////////////////////////////////////////////////////////////////////

AnanasProject::AnanasProject(Manager *manager, const QString &fileName)
    : m_manager(manager),
      m_fileName(fileName)
{
    QFileInfo fileInfo(m_fileName);
    m_projectName = fileInfo.completeBaseName();

    m_file = new AnanasProjectFile(this, fileName);
    m_rootNode = new AnanasProjectNode(this, m_file);

    m_manager->registerProject(this);
}

AnanasProject::~AnanasProject()
{
    m_manager->unregisterProject(this);

    delete m_rootNode;
}

QDir AnanasProject::projectDir() const
{
    return QFileInfo(file()->fileName()).dir();
}

QString AnanasProject::filesFileName() const
{ return m_fileName; }

static QStringList readLines(const QString &absoluteFileName)
{
    QStringList lines;

    QFile file(absoluteFileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);

        forever {
            QString line = stream.readLine();
            if (line.isNull())
                break;

            line = line.trimmed();
            if (line.isEmpty())
                continue;

            lines.append(line);
        }
    }

    return lines;
}


void AnanasProject::parseProject(RefreshOptions options)
{
    if (options & Files) {
        m_files = convertToAbsoluteFiles(readLines(filesFileName()));
        m_files.removeDuplicates();
    }

    if (options & Configuration) {
        // update configuration
    }

    if (options & Files)
        emit fileListChanged();
}

void AnanasProject::refresh(RefreshOptions options)
{
    QSet<QString> oldFileList;
    if (!(options & Configuration))
        oldFileList = m_files.toSet();

    parseProject(options);

    if (options & Files)
        m_rootNode->refresh();
}

QStringList AnanasProject::convertToAbsoluteFiles(const QStringList &paths) const
{
    const QDir projectDir(QFileInfo(m_fileName).dir());
    QStringList absolutePaths;
    foreach (const QString &file, paths) {
        QFileInfo fileInfo(projectDir, file);
        absolutePaths.append(fileInfo.absoluteFilePath());
    }
    absolutePaths.removeDuplicates();
    return absolutePaths;
}

QStringList AnanasProject::files() const
{ return m_files; }

QString AnanasProject::buildParser(const QString &) const
{
    return QString();
}

QString AnanasProject::name() const
{
    return m_projectName;
}

Core::IFile *AnanasProject::file() const
{
    return m_file;
}

Manager *AnanasProject::projectManager() const
{
    return m_manager;
}

QList<ProjectExplorer::Project *> AnanasProject::dependsOn()
{
    return QList<Project *>();
}

bool AnanasProject::isApplication() const
{
    return true;
}

bool AnanasProject::hasBuildSettings() const
{
    return false;
}

ProjectExplorer::IBuildConfigurationFactory *AnanasProject::buildConfigurationFactory() const
{
    return 0;
}

ProjectExplorer::Environment AnanasProject::environment(ProjectExplorer::BuildConfiguration *configuration) const
{
    Q_UNUSED(configuration)
    return ProjectExplorer::Environment::systemEnvironment();
}


QString AnanasProject::buildDirectory(ProjectExplorer::BuildConfiguration *configuration) const
{
    Q_UNUSED(configuration)
    return QString();
}

ProjectExplorer::BuildConfigWidget *AnanasProject::createConfigWidget()
{
    return 0;
}

QList<ProjectExplorer::BuildConfigWidget*> AnanasProject::subConfigWidgets()
{
    return QList<ProjectExplorer::BuildConfigWidget*>();
}

void AnanasProject::newBuildConfiguration(const QString &)
{
}

AnanasProjectNode *AnanasProject::rootProjectNode() const
{
    return m_rootNode;
}

QStringList AnanasProject::files(FilesMode) const
{
    return m_files;
}

QStringList AnanasProject::targets() const
{
    QStringList targets;
    return targets;
}

AnanasMakeStep *AnanasProject::makeStep() const
{
    foreach (ProjectExplorer::BuildStep *bs, buildSteps()) {
        if (AnanasMakeStep *ms = qobject_cast<AnanasMakeStep *>(bs))
            return ms;
    }
    return 0;
}

bool AnanasProject::restoreSettingsImpl(ProjectExplorer::PersistentSettingsReader &reader)
{
    Project::restoreSettingsImpl(reader);

    if (runConfigurations().isEmpty()) {
        QSharedPointer<AnanasRunConfiguration> runConf(new AnanasRunConfiguration(this));
        addRunConfiguration(runConf);
    }

    if (buildSteps().isEmpty()) {
        AnanasMakeStep *makeStep = new AnanasMakeStep(this);
        insertBuildStep(0, makeStep);
    }

    refresh(Everything);
    return true;
}

void AnanasProject::saveSettingsImpl(ProjectExplorer::PersistentSettingsWriter &writer)
{
    Project::saveSettingsImpl(writer);
}

////////////////////////////////////////////////////////////////////////////////////
// AnanasProjectFile
////////////////////////////////////////////////////////////////////////////////////

AnanasProjectFile::AnanasProjectFile(AnanasProject *parent, QString fileName)
    : Core::IFile(parent),
      m_project(parent),
      m_fileName(fileName)
{ }

AnanasProjectFile::~AnanasProjectFile()
{ }

bool AnanasProjectFile::save(const QString &)
{
    return false;
}

QString AnanasProjectFile::fileName() const
{
    return m_fileName;
}

QString AnanasProjectFile::defaultPath() const
{
    return QString();
}

QString AnanasProjectFile::suggestedFileName() const
{
    return QString();
}

QString AnanasProjectFile::mimeType() const
{
    return Constants::ANANASMIMETYPE;
}

bool AnanasProjectFile::isModified() const
{
    return false;
}

bool AnanasProjectFile::isReadOnly() const
{
    return true;
}

bool AnanasProjectFile::isSaveAsAllowed() const
{
    return false;
}

void AnanasProjectFile::modified(ReloadBehavior *)
{
}

AnanasRunConfiguration::AnanasRunConfiguration(AnanasProject *pro)
    : ProjectExplorer::LocalApplicationRunConfiguration(pro),
      m_project(pro),
      m_type(Constants::ANANASRUNCONFIGURATION)
{
    setName(tr("Ananas Viewer"));

    m_qmlViewer = Utils::SynchronousProcess::locateBinary(QLatin1String("ananasviewer"));
    //Utils::SynchronousProcess::locateBinary(searchPath, QLatin1String("qmlviewer"));
}

AnanasRunConfiguration::~AnanasRunConfiguration()
{
}

QString AnanasRunConfiguration::type() const
{
    return m_type;
}

QString AnanasRunConfiguration::executable() const
{
//    if (! QFile::exists(m_qmlViewer)) {
//        QMessageBox::information(Core::ICore::instance()->mainWindow(),
//                                 tr("Ananas Viewer"),
//                                 tr("Could not find the qmlviewer executable, please specify one."));
//    }

    return m_qmlViewer;
}

AnanasRunConfiguration::RunMode AnanasRunConfiguration::runMode() const
{
    return Gui;
}

QString AnanasRunConfiguration::workingDirectory() const
{
    QFileInfo projectFile(m_project->file()->fileName());
    return projectFile.absolutePath();
}

QStringList AnanasRunConfiguration::commandLineArguments() const
{
    QStringList args;

    const QString s = mainScript();
    if (! s.isEmpty())
        args.append(s);

    return args;
}

ProjectExplorer::Environment AnanasRunConfiguration::environment() const
{
    return ProjectExplorer::Environment::systemEnvironment();
}

QString AnanasRunConfiguration::dumperLibrary() const
{
    return QString();
}

QStringList AnanasRunConfiguration::dumperLibraryLocations() const
{
    return QStringList();
}

QWidget *AnanasRunConfiguration::configurationWidget()
{
    QWidget *config = new QWidget;
    QFormLayout *form = new QFormLayout(config);

    QComboBox *combo = new QComboBox;

    QDir projectDir = m_project->projectDir();
    QStringList files;

    files.append(tr("<Current File>"));

    int currentIndex = -1;

    foreach (const QString &fn, m_project->files()) {
        QFileInfo fileInfo(fn);
        if (fileInfo.suffix() != QLatin1String("qml"))
            continue;

        QString fileName = projectDir.relativeFilePath(fn);
        if (fileName == m_scriptFile)
            currentIndex = files.size();

        files.append(fileName);
    }

    combo->addItems(files);
    if (currentIndex != -1)
        combo->setCurrentIndex(currentIndex);

    connect(combo, SIGNAL(activated(QString)), this, SLOT(setMainScript(QString)));

    Utils::PathChooser *qmlViewer = new Utils::PathChooser;
    qmlViewer->setExpectedKind(Utils::PathChooser::Command);
    qmlViewer->setPath(executable());
    connect(qmlViewer, SIGNAL(changed(QString)), this, SLOT(onQmlViewerChanged()));

    QLineEdit *qmlViewerArgs = new QLineEdit;
    qmlViewerArgs->setText(m_qmlViewerArgs);
    connect(qmlViewerArgs, SIGNAL(textChanged(QString)), this, SLOT(onQmlViewerArgsChanged()));


    form->addRow(tr("Ananas Viewer"), qmlViewer);
    form->addRow(tr("Main Ananas File:"), combo);

    return config;
}

QString AnanasRunConfiguration::mainScript() const
{
    if (m_scriptFile.isEmpty() || m_scriptFile == tr("<Current File>")) {
        Core::EditorManager *editorManager = Core::ICore::instance()->editorManager();
        if (Core::IEditor *editor = editorManager->currentEditor()) {
            return editor->file()->fileName();
        }
    }

    return m_project->projectDir().absoluteFilePath(m_scriptFile);
}

void AnanasRunConfiguration::setMainScript(const QString &scriptFile)
{
    m_scriptFile = scriptFile;
}

void AnanasRunConfiguration::onQmlViewerChanged()
{
    if (Utils::PathChooser *chooser = qobject_cast<Utils::PathChooser *>(sender())) {
        m_qmlViewer = chooser->path();
    }
}

void AnanasRunConfiguration::save(ProjectExplorer::PersistentSettingsWriter &writer) const
{
    ProjectExplorer::LocalApplicationRunConfiguration::save(writer);

    writer.saveValue(QLatin1String("ananasviewer"), m_qmlViewer);
    writer.saveValue(QLatin1String("mainscript"), m_scriptFile);
}

void AnanasRunConfiguration::restore(const ProjectExplorer::PersistentSettingsReader &reader)
{
    ProjectExplorer::LocalApplicationRunConfiguration::restore(reader);

    m_qmlViewer = reader.restoreValue(QLatin1String("ananasviewer")).toString();
    m_scriptFile = reader.restoreValue(QLatin1String("mainscript")).toString();

    if (m_qmlViewer.isEmpty())
        m_qmlViewer = Utils::SynchronousProcess::locateBinary(QLatin1String("ananasviewer"));

    if (m_scriptFile.isEmpty())
        m_scriptFile = tr("<Current File>");
}

AnanasRunConfigurationFactory::AnanasRunConfigurationFactory()
    : m_type(Constants::ANANASRUNCONFIGURATION)
{
}

AnanasRunConfigurationFactory::~AnanasRunConfigurationFactory()
{
}

bool AnanasRunConfigurationFactory::canRestore(const QString &type) const
{
    if (type.startsWith(m_type))
        return true;

    return false;
}

QStringList AnanasRunConfigurationFactory::availableCreationTypes(ProjectExplorer::Project *) const
{
    return QStringList();
}

QString AnanasRunConfigurationFactory::displayNameForType(const QString &type) const
{
    return type;
}

QSharedPointer<ProjectExplorer::RunConfiguration> AnanasRunConfigurationFactory::create(ProjectExplorer::Project *project,
                                                                                     const QString &)
{
    AnanasProject *pro = qobject_cast<AnanasProject *>(project);
    QSharedPointer<ProjectExplorer::RunConfiguration> rc(new AnanasRunConfiguration(pro));
    return rc;
}


