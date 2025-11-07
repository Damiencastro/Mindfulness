#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>
#include <QList>
#include "Application.h" // Include the new domain entity

// Forward declarations for UI elements
class QListWidget;
class QGroupBox;

/**
 * @class ConfigWindow
 * @brief A passive presentation window that displays lists of categorized applications.
 *
 * This window is a "dumb" component. It does not fetch, scan, or save
 * any data. It simply displays the two lists of Application objects
 * that it is given in its constructor.
 */
class ConfigWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the configuration window.
     * @param games A list of Applications categorized as "Game" or "Leisure".
     * @param otherApps A list of all other categorized Applications (Work, Util, etc.).
     * @param parent The parent widget.
     */
    explicit ConfigWindow(
        QList<Application*>& games, 
        QList<Application*>& otherApps, 
        QWidget *parent = nullptr
    );
    
    ~ConfigWindow();

private:
    /**
     * @brief Private helper to populate a QListWidget with Application data.
     * @param listWidget The UI widget to fill.
     * @param applications The list of Application objects to display.
     */
    void populateList(QListWidget* listWidget, QList<Application*>& applications);

    // --- UI Elements ---
    QListWidget* m_gameListWidget;
    QListWidget* m_appListWidget;
    
    QGroupBox* m_gameBox;
    QGroupBox* m_appBox;
};

#endif // CONFIGWINDOW_H
