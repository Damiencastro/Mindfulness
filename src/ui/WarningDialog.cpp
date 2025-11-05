#include "WarningDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>

WarningDialog::WarningDialog(int minutesLeft, QWidget *parent)
    : QDialog(parent),
      m_choice(UserChoice::OK),
      m_messageLabel(nullptr)
{
    // TODO: Implement - Build UI
    // setWindowTitle("Time Warning");
    // setModal(true);
    // 
    // m_messageLabel = new QLabel(QString("%1 minutes remaining.").arg(minutesLeft));
    //
    // QDialogButtonBox* buttonBox = new QDialogButtonBox();
    // buttonBox->addButton("OK", QDialogButtonBox::AcceptRole);
    // buttonBox->addButton("Extend Time", QDialogButtonBox::ApplyRole);
    //
    // connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    // connect(buttonBox->button(QDialogButtonBox::ApplyRole), &QPushButton::clicked, this, &WarningDialog::onExtend);
    //
    // QVBoxLayout* layout = new QVBoxLayout(this);
    // layout->addWidget(m_messageLabel);
    // layout->addWidget(buttonBox);
}

WarningDialog::~WarningDialog()
{
    // TODO: Implement
}

WarningDialog::UserChoice WarningDialog::getChoice() const
{
    return m_choice;
}

void WarningDialog::onExtend()
{
    // TODO: Implement
    // m_choice = UserChoice::Extend;
    // accept();
}