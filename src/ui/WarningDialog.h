#ifndef WARNINGDIALOG_H
#define WARNINGDIALOG_H

#include <QDialog>

// Forward declarations
class QLabel;

class WarningDialog : public QDialog
{
    Q_OBJECT

public:
    enum UserChoice {
        OK,
        Extend
    };

    explicit WarningDialog(int minutesLeft, QWidget *parent = nullptr);
    ~WarningDialog();

    UserChoice getChoice() const;

private slots:
    void onExtend();

private:
    UserChoice m_choice;
    QLabel* m_messageLabel;
};

#endif // WARNINGDIALOG_H