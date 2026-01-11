// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QVBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QWebEngineView>
#include <QLoggingCategory>
#include "ui_DlgWebAuth.h"

#include "DlgWebAuth.h"
static Q_LOGGING_CATEGORY(log, "WebBrowser.Dlg.Auth")
CDlgWebAuth::CDlgWebAuth(QWebEngineWebAuthUxRequest *request, QWidget *parent)
    : QDialog(parent), uxRequest(request), ui(new Ui::CDlgWebAuth)
{
    qDebug(log) << Q_FUNC_INFO;
    ui->setupUi(this);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    scrollArea = new QScrollArea(this);
    selectAccountWidget = new QWidget(this);
    scrollArea->setWidget(selectAccountWidget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    selectAccountWidget->resize(400, 150);
    selectAccountLayout = new QVBoxLayout(selectAccountWidget);
    ui->m_mainVerticalLayout->addWidget(scrollArea);
    selectAccountLayout->setAlignment(Qt::AlignTop);

    updateDisplay();

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
            qOverload<>(&CDlgWebAuth::onCancelRequest));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
            qOverload<>(&CDlgWebAuth::onAcceptRequest));
    QAbstractButton *button = ui->buttonBox->button(QDialogButtonBox::Retry);
    connect(button, &QAbstractButton::clicked, this, qOverload<>(&CDlgWebAuth::onRetry));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

CDlgWebAuth::~CDlgWebAuth()
{
    qDebug(log) << Q_FUNC_INFO;
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    auto itr = buttons.begin();
    while (itr != buttons.end()) {
        QAbstractButton *radioButton = *itr;
        delete radioButton;
        itr++;
    }

    if (buttonGroup) {
        delete buttonGroup;
        buttonGroup = nullptr;
    }

    if (ui) {
        delete ui;
        ui = nullptr;
    }

    // selectAccountWidget and it's children will get deleted when scroll area is destroyed
    if (scrollArea) {
        delete scrollArea;
        scrollArea = nullptr;
    }
}

void CDlgWebAuth::updateDisplay()
{
    switch (uxRequest->state()) {
    case QWebEngineWebAuthUxRequest::WebAuthUxState::SelectAccount:
        setupSelectAccountUI();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::CollectPin:
        setupCollectPinUI();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::FinishTokenCollection:
        setupFinishCollectTokenUI();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::RequestFailed:
        setupErrorUI();
        break;
    default:
        break;
    }
    adjustSize();
}

void CDlgWebAuth::setupSelectAccountUI()
{
    ui->m_headingLabel->setText(tr("Choose a Passkey"));
    ui->m_description->setText(tr("Which passkey do you want to use for ")
                                            + uxRequest->relyingPartyId() + tr("? "));
    ui->m_pinGroupBox->setVisible(false);
    ui->m_mainVerticalLayout->removeWidget(ui->m_pinGroupBox);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setVisible(false);

    clearSelectAccountButtons();
    scrollArea->setVisible(true);
    selectAccountWidget->resize(this->width(), this->height());
    QStringList userNames = uxRequest->userNames();
    // Create radio buttons for each name
    for (const QString &name : userNames) {
        QRadioButton *radioButton = new QRadioButton(name);
        selectAccountLayout->addWidget(radioButton);
        buttonGroup->addButton(radioButton);
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setVisible(false);
}

void CDlgWebAuth::setupFinishCollectTokenUI()
{
    clearSelectAccountButtons();
    ui->m_headingLabel->setText(tr("Use your security key with")
                                             + uxRequest->relyingPartyId());
    ui->m_description->setText(
            tr("Touch your security key again to complete the request."));
    ui->m_pinGroupBox->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setVisible(false);
    scrollArea->setVisible(false);
}
void CDlgWebAuth::setupCollectPinUI()
{
    clearSelectAccountButtons();
    ui->m_mainVerticalLayout->addWidget(ui->m_pinGroupBox);
    ui->m_pinGroupBox->setVisible(true);
    ui->m_confirmPinLabel->setVisible(false);
    ui->m_confirmPinLineEdit->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Next"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setVisible(false);
    scrollArea->setVisible(false);

    QWebEngineWebAuthPinRequest pinRequestInfo = uxRequest->pinRequest();

    if (pinRequestInfo.reason == QWebEngineWebAuthUxRequest::PinEntryReason::Challenge) {
        ui->m_headingLabel->setText(tr("PIN Required"));
        ui->m_description->setText(tr("Enter the PIN for your security key"));
        ui->m_confirmPinLabel->setVisible(false);
        ui->m_confirmPinLineEdit->setVisible(false);
    } else {
        if (pinRequestInfo.reason == QWebEngineWebAuthUxRequest::PinEntryReason::Set) {
            ui->m_headingLabel->setText(tr("New PIN Required"));
            ui->m_description->setText(tr("Set new PIN for your security key"));
        } else {
            ui->m_headingLabel->setText(tr("Change PIN Required"));
            ui->m_description->setText(tr("Change PIN for your security key"));
        }
        ui->m_confirmPinLabel->setVisible(true);
        ui->m_confirmPinLineEdit->setVisible(true);
    }

    QString errorDetails;
    switch (pinRequestInfo.error) {
    case QWebEngineWebAuthUxRequest::PinEntryError::NoError:
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::InternalUvLocked:
        errorDetails = tr("Internal User Verification Locked ");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::WrongPin:
        errorDetails = tr("Wrong PIN");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::TooShort:
        errorDetails = tr("Too Short");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::InvalidCharacters:
        errorDetails = tr("Invalid Characters");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::SameAsCurrentPin:
        errorDetails = tr("Same as current PIN");
        break;
    }
    if (!errorDetails.isEmpty()) {
        errorDetails += tr(" ") + QString::number(pinRequestInfo.remainingAttempts)
                + tr(" attempts remaining");
    }
    ui->m_pinEntryErrorLabel->setText(errorDetails);
}

void CDlgWebAuth::onCancelRequest()
{
    uxRequest->cancel();
}

void CDlgWebAuth::onAcceptRequest()
{
    switch (uxRequest->state()) {
    case QWebEngineWebAuthUxRequest::WebAuthUxState::SelectAccount:
        if (buttonGroup->checkedButton()) {
            uxRequest->setSelectedAccount(buttonGroup->checkedButton()->text());
        }
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::CollectPin:
        uxRequest->setPin(ui->m_pinLineEdit->text());
        break;
    default:
        break;
    }
}

void CDlgWebAuth::setupErrorUI()
{
    clearSelectAccountButtons();
    QString errorDescription;
    QString errorHeading = tr("Something went wrong");
    bool isVisibleRetry = false;
    switch (uxRequest->requestFailureReason()) {
    case QWebEngineWebAuthUxRequest::RequestFailureReason::Timeout:
        errorDescription = tr("Request Timeout");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyNotRegistered:
        errorDescription = tr("Key not registered");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyAlreadyRegistered:
        errorDescription = tr("You already registered this device."
                              "Try again with device");
        isVisibleRetry = true;
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::SoftPinBlock:
        errorDescription =
                tr("The security key is locked because the wrong PIN was entered too many times."
                   "To unlock it, remove and reinsert it.");
        isVisibleRetry = true;
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::HardPinBlock:
        errorDescription =
                tr("The security key is locked because the wrong PIN was entered too many times."
                   " You'll need to reset the security key.");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorRemovedDuringPinEntry:
        errorDescription =
                tr("Authenticator removed during verification. Please reinsert and try again");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingResidentKeys:
        errorDescription = tr("Authenticator doesn't have resident key support");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingUserVerification:
        errorDescription = tr("Authenticator missing user verification");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingLargeBlob:
        errorDescription = tr("Authenticator missing Large Blob support");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::NoCommonAlgorithms:
        errorDescription = tr("Authenticator missing Large Blob support");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::StorageFull:
        errorDescription = tr("Storage Full");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::UserConsentDenied:
        errorDescription = tr("User consent denied");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::WinUserCancelled:
        errorDescription = tr("User Cancelled Request");
        break;
    }

    ui->m_headingLabel->setText(errorHeading);
    ui->m_description->setText(errorDescription);
    ui->m_description->adjustSize();
    ui->m_pinGroupBox->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setVisible(isVisibleRetry);
    if (isVisibleRetry)
        ui->buttonBox->button(QDialogButtonBox::Retry)->setFocus();
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
    scrollArea->setVisible(false);
}

void CDlgWebAuth::onRetry()
{
    uxRequest->retry();
}

void CDlgWebAuth::clearSelectAccountButtons()
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    auto itr = buttons.begin();
    while (itr != buttons.end()) {
        QAbstractButton *radioButton = *itr;
        selectAccountLayout->removeWidget(radioButton);
        buttonGroup->removeButton(radioButton);
        delete radioButton;
        itr++;
    }
}
