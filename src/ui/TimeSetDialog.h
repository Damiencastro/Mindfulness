#ifndef TIMESETDIALOG_H
#define TIMESETDIALOG_H

#include <QDialog>

// Forward declarations
class QSpinBox;

class TimeSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeSetDialog(QWidget *parent = nullptr);
    ~TimeSetDialog();

    int getSelectedTime() const;

signals:
    void timeSelected(int minutes);

private slots:
    void onAccept();

private:
    QSpinBox* m_timeSpinBox;
};

#endif // TIMESETDIALOG_H