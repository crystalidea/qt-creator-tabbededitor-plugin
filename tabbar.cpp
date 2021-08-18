#include "tabbar.h"

#include "tabbededitorconstants.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/idocument.h>
#include <projectexplorer/session.h>

#include <QMenu>
#include <QTimer>
#include <QMouseEvent>
#include <QShortcut>
#include <QTabBar>
#include <QFile>

#include "include/plog/Log.h"

using namespace Core::Internal;

using namespace TabbedEditor::Internal;

/// TODO: Use Core::DocumentModel for everything

TabBar::TabBar(QWidget* parent) :
    QTabBar(parent)
{
    setExpanding(false);
    setMovable(true);
    setTabsClosable(true);
    setUsesScrollButtons(true);

    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sp.setHorizontalStretch(1);
    sp.setVerticalStretch(0);
    sp.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(sp);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_pTimer->start(1000);

    connect(this, &QTabBar::tabMoved, [this](int from, int to)
    {
        PLOGD << "from:" << from << ",displayName: " << m_editors.at(from)->document()->displayName();
        PLOGD << "to:" << to << ",displayName: " << m_editors.at(to)->document()->displayName();

        m_editors.move(from, to);
    });

    Core::EditorManager* em = Core::EditorManager::instance();

    connect(em, &Core::EditorManager::editorOpened, this, &TabBar::addEditorTab);
    connect(em, &Core::EditorManager::editorsClosed, this, &TabBar::removeEditorTabs);
    connect(em, SIGNAL(currentEditorChanged(Core::IEditor*)), SLOT(selectEditorTab(Core::IEditor*)));

    connect(em, SIGNAL(documentStateChanged(Core::IDocument*)), SLOT(PinnedChanged(Core::IDocument*)));

    connect(this, &QTabBar::currentChanged, this, &TabBar::activateEditor);

    connect(this, &QTabBar::tabCloseRequested, this, &TabBar::closeTab);

    ProjectExplorer::SessionManager* sm = ProjectExplorer::SessionManager::instance();
    connect(sm, &ProjectExplorer::SessionManager::sessionLoaded, [this, em]()
    {
        foreach (Core::DocumentModel::Entry* entry, Core::DocumentModel::entries())
        {
            em->activateEditorForEntry(entry, Core::EditorManager::DoNotChangeCurrentEditor);
        }
    });

    const QString shortCutSequence = QStringLiteral("Ctrl+Alt+%1");
    for (int i = 1; i <= 10; ++i)
    {
        QShortcut* shortcut = new QShortcut(shortCutSequence.arg(i % 10), this);
        connect(shortcut, &QShortcut::activated, [this, shortcut]()
        {
            setCurrentIndex(m_shortcuts.indexOf(shortcut));
        });
        m_shortcuts.append(shortcut);
    }

    QAction* prevTabAction = new QAction(tr("Switch to previous tab"), this);
    Core::Command* prevTabCommand
        = Core::ActionManager::registerAction(prevTabAction,
                TabbedEditor::Constants::PREV_TAB_ID,
                Core::Context(Core::Constants::C_GLOBAL));
    prevTabCommand->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+J")));
    connect(prevTabAction, SIGNAL(triggered()), this, SLOT(prevTabAction()));

    QAction* nextTabAction = new QAction(tr("Switch to next tab"), this);
    Core::Command* nextTabCommand
        = Core::ActionManager::registerAction(nextTabAction,
                TabbedEditor::Constants::NEXT_TAB_ID,
                Core::Context(Core::Constants::C_GLOBAL));
    nextTabCommand->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+K")));
    connect(nextTabAction, SIGNAL(triggered()), this, SLOT(nextTabAction()));
}

void TabBar::addEditorTab(Core::IEditor* editor)
{
    PLOGD << "add displayName :" << editor->document()->displayName();
    const uint PINED_MAX_TAB = 9;

    if(m_editors.size() >= PINED_MAX_TAB)
    {

        foreach (Core::IEditor* editor, m_editors)
        {
            const int index = m_editors.indexOf(editor);
            if (index == -1)
            {
                continue;
            }

            Core::IDocument* document = editor->document();
            Core::DocumentModel::Entry* entry = Core::DocumentModel::entryForDocument(document);
            if(entry && !entry->pinned)
            {
                PLOGD << "m_editors.size()  >  PINED_MAX_TAB ,m_PinedCount:" << m_PinedCount << ", unPined displayName;" << document->displayName();
                closeTab(index);
                break;
            }
        }

    }

    Core::IDocument* document = editor->document();

    const int index = addTab(document->displayName());
    PLOGD << "index:" << index << ",m_PinedCount:" << m_PinedCount << ",name:" << document->displayName().toStdString();

    setTabIcon(index, Core::FileIconProvider::icon(document->filePath().toFileInfo()));
    setTabToolTip(index, document->filePath().toString());

    if(m_PinedCount < PINED_MAX_TAB)
    {
        Core::DocumentModel::Entry* entry = Core::DocumentModel::entryForDocument(document);
        if(entry && entry->pinned)
        {
            m_PinedCount++;
        }
    }

    m_editors.append(editor);
    connect(document, &Core::IDocument::changed, [this, editor, document]()
    {
        const int index = m_editors.indexOf(editor);
        if (index == -1)
        {
            return;
        }
        QString tabText = document->displayName();

        if (document->isModified())
        {
            tabText += QLatin1Char('*');
        }

        PLOGD << "Core::IDocument::changed index:" << index << ",displayName:" << editor->document()->displayName() << ",newtabText:" << tabText.toStdString();
        setTabText(index, tabText);
    });
}

void TabBar::removeEditorTabs(QList<Core::IEditor*> editors)
{
    blockSignals(true); // Avoid calling activateEditor()
    foreach (Core::IEditor* editor, editors)
    {
        const int index = m_editors.indexOf(editor);
        PLOGD << "index:" << index << ",displayName:" << editor->document()->displayName();
        if (index == -1)
        {
            continue;
        }

        m_editors.removeAt(index);
        removeTab(index);
    }
    blockSignals(false);
}

void TabBar::activateEditor(int index)
{
    PLOGD << index;

    if (index < 0 || index >= m_editors.size())
    {
        return;
    }

    Core::EditorManager::instance()->activateEditor(m_editors[index]);
}

void TabBar::selectEditorTab(Core::IEditor* editor)
{
    const int index = m_editors.indexOf(editor);
    if (index == -1)
    {
        return;
    }
    setCurrentIndex(index);
}

void TabBar::closeTab(int index)
{
    if (index < 0 || index >= m_editors.size())
    {
        return;
    }

    Core::DocumentModel::Entry* entry = Core::DocumentModel::entryForDocument(m_editors.at(index)->document());
    if(entry && entry->pinned)
    {
        return;
    }
    const QList<Core::IEditor *> editorsToClose = {m_editors.at(index)};
    Core::EditorManager::instance()->closeEditors(editorsToClose,true);
    //removeTab(index); //do not add this line back into Code it is redundand ! The tab is removed by the tabbar itself! Otherwise you get wierd mechanics like tabs been removed with still editors left open!
}

void TabBar::prevTabAction()
{
    int index = currentIndex();
    if (index >= 1)
    {
        setCurrentIndex(index - 1);
    }
    else
    {
        setCurrentIndex(count() - 1);
    }
}

void TabBar::nextTabAction()
{
    int index = currentIndex();
    if (index < count() - 1)
    {
        setCurrentIndex(index + 1);
    }
    else
    {
        setCurrentIndex(0);
    }
}

void TabBar::handleTimeout()
{
    PLOGD << "handleTimeout**:";
    blockSignals(true); // Avoid calling activateEditor()
    m_PinedCount = 0;
    foreach (Core::IEditor* editor, m_editors)
    {
        const int index = m_editors.indexOf(editor);
        if (index == -1)
        {
            continue;
        }

        Core::IDocument* document = editor->document();
        Core::DocumentModel::Entry* entry = Core::DocumentModel::entryForDocument(document);
        if(entry)
        {

            auto qf = QFileInfo(document->displayName());
            std::string fn = qf.fileName().toStdString();
            if(fn.length() > 15)
            {
                std::string pre = fn.substr(0, 6);
                std::string suff = fn.substr(fn.length() - 6, 6);
                fn = pre + "..." + suff;
            }

            QString dtabText = tabText(index);

            QString text = QString::fromStdString (fn);
            setTabText(index, text);

            if(dtabText.toStdString().find("*") != std::string::npos)
            {
                text += QLatin1Char('*');
                setTabText(index, text);
            }

            if(entry->pinned)
            {
                text += QLatin1Char('#');
                setTabText(index, text);

                moveTab(index, m_PinedCount);

                m_editors.move(index, m_PinedCount);

                // emit tabMoved(index, m_PinedCount);

                m_PinedCount++;
            }

        }
    }
    blockSignals(false);
}

void TabBar::contextMenuEvent(QContextMenuEvent* event)
{
    const int index = tabAt(event->pos());
    if (index == -1)
    {
        return;
    }

    QScopedPointer<QMenu> menu(new QMenu());

    Core::IEditor* editor = m_editors[index];
    Core::DocumentModel::Entry* entry = Core::DocumentModel::entryForDocument(editor->document());

    Core::EditorManager::addPinEditorActions(menu.data(), entry);

    menu->addSeparator();

    Core::EditorManager::addSaveAndCloseEditorActions(menu.data(), entry, editor);

    menu->addSeparator();
    Core::EditorManager::addNativeDirAndOpenWithActions(menu.data(), entry);

    menu->exec(mapToGlobal(event->pos()));
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        closeTab(tabAt(event->pos()));
    }
    QTabBar::mouseReleaseEvent(event);
}
