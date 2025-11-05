#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>

// Forward declarations
class GameList;
class QListWidget;
class QPushButton;

class ConfigWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWindow(GameList* gameList, QWidget *parent = nullptr);
    ~ConfigWindow();

    void highlightProcess(const QString& processName);

private slots:
    void onSave();
    void onRefresh();

private:
    void populateList();

    GameList* m_gameList; // Pointer, not owned
    
    // UI Elements
    QListWidget* m_listWidget;
    QPushButton* m_saveButton;
    QPushButton* m_refreshButton;
};

#endif // CONFIGWINDOW_H