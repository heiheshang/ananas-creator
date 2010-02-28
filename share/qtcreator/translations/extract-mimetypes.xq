let $files := ( "/home/oper/Documents/ananas/ananas-creator/src/plugins/ananasprojectmanager/AnanasProject.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/bineditor/BinEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/cmakeprojectmanager/CMakeProject.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/cppeditor/CppEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/cvs/CVS.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/designer/Designer.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/directoryeditor/DirectoryEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/duieditor/DuiEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/fieldeditor/FieldEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/genericprojectmanager/GenericProject.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/journaleditor/JournalEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/perforce/Perforce.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/qmleditor/QmlEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/qmlprojectmanager/QmlProject.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/qt4projectmanager/Qt4ProjectManager.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/qtscripteditor/QtScriptEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/resourceeditor/ResourceEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/subversion/Subversion.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/texteditor/TextEditor.mimetypes.xml", "/home/oper/Documents/ananas/ananas-creator/src/plugins/vcsbase/VCSBase.mimetypes.xml" )
let $prefix := string("QT_TRANSLATE_NOOP(&quot;MimeType&quot;, &quot;")
let $suffix := concat("&quot;)", codepoints-to-string(10))
for $file in $files
    for $comment in doc($file)/*:mime-info/*:mime-type/*:comment
        return fn:concat($prefix, data($comment), $suffix)
