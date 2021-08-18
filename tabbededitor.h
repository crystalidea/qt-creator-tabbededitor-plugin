#ifndef TABBEDEDITORPLUGIN_H
#define TABBEDEDITORPLUGIN_H

#include "tabbededitor_global.h"

#include <extensionsystem/iplugin.h>

namespace TabbedEditor {
namespace Internal {

class TabBar;

class TabbedEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TabbedEditor.json")

public:
    TabbedEditorPlugin();
    ~TabbedEditorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;
    bool delayedInitialize() ;

private slots:
    QString getStylesheetPatternFromFile(const QString& filepath);
    void updateStyleToBaseColor();
    void showTabBar();

private:
    static inline QString getQssStringFromColor(const QColor& color);

        TabBar* m_tabBar;
        bool m_styleUpdatedToBaseColor;
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABBEDEDITORPLUGIN_H
