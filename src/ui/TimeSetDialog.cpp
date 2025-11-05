#include "TimeSetDialog.h"
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

TimeSetDialog::TimeSetDialog(QWidget *parent)
    : QDialog(parent),
      m_timeSpinBox(nullptr)
{
    // TODO: Implement - Build UI
    // m_timeSpinBox = new QSpinBox();
    // m_timeSpinBox->setRange(5, 240); // 5 min to 4 hours
    // m_timeSpinBox->setValue(30);
    //
    // QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    //
    // QVBoxLayout* layout = new QVBoxLayout(this);
    // layout->addWidget(m_timeSpinBox);
    // layout->addWidget(buttonBox);
    //
    // connect(buttonBox, &QDialogButtonBox::accepted, this, &TimeSetDialog::onAccept);
    // connect(buttonBox, &QDialogButtonBox::rejected, this, &TimeSetDialog::reject);
}

TimeSetDialog::~TimeSetDialog()
{
    // TODO: Implement
}

int TimeSetDialog::getSelectedTime() const
{
    // TODO: Implement
    // if (m_timeSpinBox) {
    //     return m_timeSpinBox->value();
    // }
    return 0;
}

void TimeSetDialog::onAccept()
{
    // TODO: Implement
    // emit timeSelected(getSelectedTime());
    // accept();
}