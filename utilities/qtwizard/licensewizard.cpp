/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "licensewizard.h"

LicenseWizard::LicenseWizard(QWidget *parent)
    : ComplexWizard(parent)
{
    titlePage = new TitlePage(this);
    evaluatePage = new EvaluatePage(this);
    registerPage = new RegisterPage(this);
    detailsPage = new DetailsPage(this);
    finishPage = new FinishPage(this);

    setFirstPage(titlePage);

    setWindowTitle(tr("Complex Wizard"));
    resize(480, 200);
}

TitlePage::TitlePage(LicenseWizard *wizard)
    : LicenseWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><font color=\"blue\" size=\"5\"><b><i>"
                             "Super Product One</i></b></font></center>"));

    registerRadioButton = new QRadioButton(tr("&Register your copy"));
    evaluateRadioButton = new QRadioButton(tr("&Evaluate our product"));
    setFocusProxy(registerRadioButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addSpacing(10);
    layout->addWidget(registerRadioButton);
    layout->addWidget(evaluateRadioButton);
    layout->addStretch(1);
    setLayout(layout);
}

void TitlePage::resetPage()
{
    registerRadioButton->setChecked(true);
}

WizardPage *TitlePage::nextPage()
{
    if (evaluateRadioButton->isChecked())
        return wizard->evaluatePage;
    else
        return wizard->registerPage;
}

EvaluatePage::EvaluatePage(LicenseWizard *wizard)
    : LicenseWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Evaluate Super Product One"
                             "</b></center>"));

    nameLabel = new QLabel(tr("&Name:"));
    nameLineEdit = new QLineEdit;
    nameLabel->setBuddy(nameLineEdit);
    setFocusProxy(nameLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit;
    emailLabel->setBuddy(emailLineEdit);

    bottomLabel = new QLabel(tr("Please fill in both fields.\nThis will "
                                "entitle you to a 30-day evaluation."));

    connect(nameLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));
    connect(emailLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(nameLabel, 2, 0);
    layout->addWidget(nameLineEdit, 2, 1);
    layout->addWidget(emailLabel, 3, 0);
    layout->addWidget(emailLineEdit, 3, 1);
    layout->setRowMinimumHeight(4, 10);
    layout->addWidget(bottomLabel, 5, 0, 1, 2);
    layout->setRowStretch(6, 1);
    setLayout(layout);
}

void EvaluatePage::resetPage()
{
    nameLineEdit->clear();
    emailLineEdit->clear();
}

WizardPage *EvaluatePage::nextPage()
{
    return wizard->finishPage;
}

bool EvaluatePage::isComplete()
{
    return !nameLineEdit->text().isEmpty() && !emailLineEdit->text().isEmpty();
}

RegisterPage::RegisterPage(LicenseWizard *wizard)
    : LicenseWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Register your copy of Super Product "
                             "One</b></center>"));

    nameLabel = new QLabel(tr("&Name:"));
    nameLineEdit = new QLineEdit;
    nameLabel->setBuddy(nameLineEdit);
    setFocusProxy(nameLineEdit);

    upgradeKeyLabel = new QLabel(tr("&Upgrade key:"));
    upgradeKeyLineEdit = new QLineEdit;
    upgradeKeyLabel->setBuddy(upgradeKeyLineEdit);

    bottomLabel = new QLabel(tr("If you have an upgrade key, please fill in "
                                "the appropriate field."));

    connect(nameLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(nameLabel, 2, 0);
    layout->addWidget(nameLineEdit, 2, 1);
    layout->addWidget(upgradeKeyLabel, 3, 0);
    layout->addWidget(upgradeKeyLineEdit, 3, 1);
    layout->setRowMinimumHeight(4, 10);
    layout->addWidget(bottomLabel, 5, 0, 1, 2);
    layout->setRowStretch(6, 1);
    setLayout(layout);
}

void RegisterPage::resetPage()
{
    nameLineEdit->clear();
    upgradeKeyLineEdit->clear();
}

WizardPage *RegisterPage::nextPage()
{
    if (upgradeKeyLineEdit->text().isEmpty())
        return wizard->detailsPage;
    else
        return wizard->finishPage;
}

bool RegisterPage::isComplete()
{
    return !nameLineEdit->text().isEmpty();
}

DetailsPage::DetailsPage(LicenseWizard *wizard)
    : LicenseWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Fill in your details</b></center>"));

    companyLabel = new QLabel(tr("&Company name:"));
    companyLineEdit = new QLineEdit;
    companyLabel->setBuddy(companyLineEdit);
    setFocusProxy(companyLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit;
    emailLabel->setBuddy(emailLineEdit);

    postalLabel = new QLabel(tr("&Postal address:"));
    postalLineEdit = new QLineEdit;
    postalLabel->setBuddy(postalLineEdit);

    connect(companyLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));
    connect(emailLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));
    connect(postalLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(companyLabel, 2, 0);
    layout->addWidget(companyLineEdit, 2, 1);
    layout->addWidget(emailLabel, 3, 0);
    layout->addWidget(emailLineEdit, 3, 1);
    layout->addWidget(postalLabel, 4, 0);
    layout->addWidget(postalLineEdit, 4, 1);
    layout->setRowStretch(5, 1);
    setLayout(layout);
}

void DetailsPage::resetPage()
{
    companyLineEdit->clear();
    emailLineEdit->clear();
    postalLineEdit->clear();
}

WizardPage *DetailsPage::nextPage()
{
    return wizard->finishPage;
}

bool DetailsPage::isComplete()
{
    return !companyLineEdit->text().isEmpty()
           && !emailLineEdit->text().isEmpty()
           && !postalLineEdit->text().isEmpty();
}

FinishPage::FinishPage(LicenseWizard *wizard)
    : LicenseWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Complete your registration"
                             "</b></center>"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    agreeCheckBox = new QCheckBox(tr("I agree to the terms and conditions of "
                                     "the license"));
    setFocusProxy(agreeCheckBox);

    connect(agreeCheckBox, SIGNAL(toggled(bool)),
            this, SIGNAL(completeStateChanged()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addSpacing(10);
    layout->addWidget(bottomLabel);
    layout->addWidget(agreeCheckBox);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::resetPage()
{
    QString licenseText;

    if (wizard->historyPages().contains(wizard->evaluatePage)) {
        licenseText = tr("Evaluation License Agreement: "
                         "You can use this software for 30 days and make one "
                         "back up, but you are not allowed to distribute it.");
    } else if (wizard->historyPages().contains(wizard->detailsPage)) {
        licenseText = tr("First-Time License Agreement: "
                         "You can use this software subject to the license "
                         "you will receive by email.");
    } else {
        licenseText = tr("Upgrade License Agreement: "
                         "This software is licensed under the terms of your "
                         "current license.");
    }
    bottomLabel->setText(licenseText);
    agreeCheckBox->setChecked(false);
}

bool FinishPage::isComplete()
{
    return agreeCheckBox->isChecked();
}
