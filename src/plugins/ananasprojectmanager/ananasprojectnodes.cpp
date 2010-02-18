

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

#include "ananasprojectnodes.h"
#include "ananasprojectmanager.h"
#include "ananasproject.h"

#include <coreplugin/ifile.h>
#include <projectexplorer/projectexplorer.h>

#include <QFileInfo>
#include <QDir>

using namespace AnanasProjectManager;
using namespace AnanasProjectManager::Internal;

AnanasProjectNode::AnanasProjectNode(AnanasProject *project, Core::IFile *projectFile)
    : ProjectExplorer::ProjectNode(QFileInfo(projectFile->fileName()).absolutePath()),
      m_project(project),
      m_projectFile(projectFile)
{
 setFolderName(QFileInfo(projectFile->fileName()).completeBaseName());
 parseRc();
 if (m_rc.contains("dbtitle"))
       setFolderName(m_rc.value("dbtitle"));

}

void AnanasProjectNode::parseRc()
{
 QFile file( m_project->filesFileName());
 if ( file.open( QIODevice::ReadOnly ) )
  {
        QTextStream stream( &file );
        QString line, key, value;

        stream.setCodec("UTF-8");
        while ( !stream.atEnd() ) {
                line = stream.readLine(); // line of text excluding '\n'
                key = line.section("=",0,0);
                value = line.section("=",1,100); if ( value.isNull() ) value = "";
                m_rc.insert(key,value);
        }
        file.close();
    }

}
AnanasProjectNode::~AnanasProjectNode()
{ }

Core::IFile *AnanasProjectNode::projectFile() const
{ return m_projectFile; }

QString AnanasProjectNode::projectFilePath() const
{ return m_projectFile->fileName(); }

void AnanasProjectNode::refresh()
{
    using namespace ProjectExplorer;

    // remove the existing nodes.
    removeFileNodes(fileNodes(), this);
    removeFolderNodes(subFolderNodes(), this);

    //ProjectExplorerPlugin::instance()->setCurrentNode(0); // ### remove me

    FileNode *projectFilesNode = new FileNode(m_project->filesFileName(),
                                              ProjectFileType,
                                              /* generated = */ false);

    QStringList files = m_project->files();
    files.removeAll(m_project->filesFileName());

    addFileNodes(QList<FileNode *>()
                 << projectFilesNode,
                 this);
    if (m_rc.contains("configfile")) {
        FileNode *configFilesNode = new FileNode(m_rc.value("configfile"),
                                              ProjectFileType,
                                              /* generated = */ false);
        addFileNodes(QList<FileNode *>()
                  << configFilesNode,
                 this);
    }
//    QStringList filePaths;
//    QHash<QString, QStringList> filesInPath;
//
//    foreach (const QString &absoluteFileName, files) {
//        QFileInfo fileInfo(absoluteFileName);
//        const QString absoluteFilePath = fileInfo.path();
//
//        if (! absoluteFilePath.startsWith(path()))
//            continue; // `file' is not part of the project.
//
//        const QString relativeFilePath = absoluteFilePath.mid(path().length() + 1);
//
//        if (! filePaths.contains(relativeFilePath))
//            filePaths.append(relativeFilePath);
//
//        filesInPath[relativeFilePath].append(absoluteFileName);
//    }

//    foreach (const QString &filePath, filePaths) {
//        FolderNode *folder = findOrCreateFolderByName(filePath);
//
//        QList<FileNode *> fileNodes;
//        foreach (const QString &file, filesInPath.value(filePath)) {
//            FileType fileType = SourceType; // ### FIXME
//            FileNode *fileNode = new FileNode(file, fileType, /*generated = */ false);
//            fileNodes.append(fileNode);
//        }
//
//        addFileNodes(fileNodes, folder);
//    }

    m_folderByName.clear();
}

ProjectExplorer::FolderNode *AnanasProjectNode::findOrCreateFolderByName(const QStringList &components, int end)
{
    if (! end)
        return 0;

    QString folderName;
    for (int i = 0; i < end; ++i) {
        folderName.append(components.at(i));
        folderName += QLatin1Char('/'); // ### FIXME
    }

    const QString component = components.at(end - 1);

    if (component.isEmpty())
        return this;

    else if (FolderNode *folder = m_folderByName.value(folderName))
        return folder;

    FolderNode *folder = new FolderNode(component);
    m_folderByName.insert(folderName, folder);

    FolderNode *parent = findOrCreateFolderByName(components, end - 1);
    if (! parent)
        parent = this;
    addFolderNodes(QList<FolderNode*>() << folder, parent);

    return folder;
}

ProjectExplorer::FolderNode *AnanasProjectNode::findOrCreateFolderByName(const QString &filePath)
{
    QStringList components = filePath.split(QLatin1Char('/'));
    return findOrCreateFolderByName(components, components.length());
}

bool AnanasProjectNode::hasTargets() const
{
    return true;
}

QList<ProjectExplorer::ProjectNode::ProjectAction> AnanasProjectNode::supportedActions() const
{
    QList<ProjectAction> actions;
    actions.append(AddFile);
    return actions;
}

bool AnanasProjectNode::addSubProjects(const QStringList &proFilePaths)
{
    Q_UNUSED(proFilePaths)
    return false;
}

bool AnanasProjectNode::removeSubProjects(const QStringList &proFilePaths)
{
    Q_UNUSED(proFilePaths)
    return false;
}

bool AnanasProjectNode::addFiles(const ProjectExplorer::FileType,
                              const QStringList &filePaths, QStringList *notAdded)
{
    QDir projectDir = QFileInfo(projectFilePath()).dir();

    QFile file(projectFilePath());
    if (! file.open(QFile::WriteOnly | QFile::Append))
        return false;

    QTextStream stream(&file);
    QStringList failedFiles;

    bool first = true;
    foreach (const QString &filePath, filePaths) {
        const QString rel = projectDir.relativeFilePath(filePath);

        if (rel.isEmpty() || rel.startsWith(QLatin1Char('.'))) {
            failedFiles.append(rel);
        } else {
            if (first) {
                stream << endl;
                first = false;
            }

            stream << rel << endl;
        }
    }

    if (notAdded)
        *notAdded += failedFiles;

    if (! first)
        m_project->projectManager()->notifyChanged(projectFilePath());

    return failedFiles.isEmpty();
}

bool AnanasProjectNode::removeFiles(const ProjectExplorer::FileType fileType,
                                 const QStringList &filePaths, QStringList *notRemoved)
{
    Q_UNUSED(fileType)
    Q_UNUSED(filePaths)
    Q_UNUSED(notRemoved)
    return false;
}

bool AnanasProjectNode::renameFile(const ProjectExplorer::FileType fileType,
                                    const QString &filePath, const QString &newFilePath)
{
    Q_UNUSED(fileType)
    Q_UNUSED(filePath)
    Q_UNUSED(newFilePath)
    return false;
}
