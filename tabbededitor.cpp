#include "tabbededitor.h"
#include "tabbededitorconstants.h"
#include "tabbar.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <utils/stylehelper.h>
#include <utils/theme/theme.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QBoxLayout>
#include <QFile>

#include "include/plog/Log.h"

namespace TabbedEditor {
namespace Internal {

TabbedEditorPlugin::TabbedEditorPlugin():
 m_tabBar(nullptr), m_styleUpdatedToBaseColor(false)
{
}

TabbedEditorPlugin::~TabbedEditorPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    // 2DO!!
}

bool TabbedEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
        Q_UNUSED(errorString)

        //./qtcreator & tail -F ./TabbedEditor.log
        plog::init(plog::debug, "TabbedEditor.log");
        PLOGD << "\n\n--------------------------------Hello TabbedEditor log!--------------------"; // short macro

        connect(Core::ICore::instance(), SIGNAL(themeChanged()), this, SLOT(updateStyleToBaseColor()));
        connect(Core::EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)), SLOT(showTabBar()));

        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(Core::ICore::mainWindow());
        mainWindow->layout()->setSpacing(0);

        QWidget* wrapper = new QWidget(mainWindow);
        wrapper->setMinimumHeight(0);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        m_tabBar = new TabBar();
        layout->addWidget(m_tabBar);
        layout->addWidget(mainWindow->centralWidget());

        wrapper->setLayout(layout);

        mainWindow->setCentralWidget(wrapper);

        return true;
}

bool TabbedEditorPlugin::delayedInitialize()
{
    m_tabBar->setCurrentIndex(0);
    return false;
}

QString TabbedEditorPlugin::getStylesheetPatternFromFile(const QString& filepath)
{
    QFile stylesheetFile(filepath);
    if (!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }
    return QString::fromUtf8(stylesheetFile.readAll());
}

void TabbedEditorPlugin::updateStyleToBaseColor()
{
    Utils::Theme* theme = Utils::creatorTheme();

    QString baseColorQss;
    QString borderColorQss;
    QString highlightColorQss;
    QString selectedTabBorderColorQss;
    QString shadowColorQss;

    if(theme->preferredStyles().isEmpty())
    {
        baseColorQss = getQssStringFromColor(Utils::StyleHelper::baseColor().lighter(130));
        borderColorQss = getQssStringFromColor(Utils::StyleHelper::borderColor());
        highlightColorQss = getQssStringFromColor(Utils::StyleHelper::baseColor());
        selectedTabBorderColorQss
            = getQssStringFromColor(Utils::StyleHelper::highlightColor().lighter());
        shadowColorQss = getQssStringFromColor(Utils::StyleHelper::shadowColor());
    }
    else     // Flat widget style
    {
        baseColorQss
            = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorHover));
        borderColorQss = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorHover));
        highlightColorQss = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorDark));
        selectedTabBorderColorQss
            = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorDark));
        shadowColorQss = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorNormal));
    }

    QString stylesheetPattern = getStylesheetPatternFromFile(QStringLiteral(":/styles/default.qss"));

    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%FRAME_BACKGROUND_COLOR%"), highlightColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_SELECTED_BORDER_COLOR%"), selectedTabBorderColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_SELECTED_BACKGROUND_COLOR%"), baseColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_SELECTED_BOTTOM_BORDER_COLOR%"), baseColorQss);

    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BACKGROUND_COLOR_FROM%"), shadowColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BACKGROUND_COLOR_TO%"), shadowColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BORDER_COLOR%"), borderColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BOTTOM_BORDER_COLOR%"), borderColorQss);

    m_tabBar->setStyleSheet(stylesheetPattern);
}

void TabbedEditorPlugin::showTabBar()
{
    updateStyleToBaseColor();

    if (m_styleUpdatedToBaseColor)
    {
        disconnect(Core::EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)),
                   this, SLOT(showTabBar()));
        return;
    }

    m_styleUpdatedToBaseColor = true;
}


void TabbedEditorPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag TabbedEditorPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

QString TabbedEditorPlugin::getQssStringFromColor(const QColor& color)
{
    return QString::fromLatin1("rgba(%1, %2, %3, %4)").arg(
               QString::number(color.red()),
               QString::number(color.green()),
               QString::number(color.blue()),
               QString::number(color.alpha()));
}

} // namespace Internal
} // namespace TabbedEditor
