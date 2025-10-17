// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#pragma once

#include <QDialog>
#include <QButtonGroup>
#include <QScrollArea>
#include <QVBoxLayout>
#include "ui_DlgWebAuth.h"
#include <QWebEngineWebAuthUxRequest>

class CDlgWebAuth : public QDialog
{
    Q_OBJECT
public:
    CDlgWebAuth(QWebEngineWebAuthUxRequest *request, QWidget *parent = nullptr);
    ~CDlgWebAuth();

    void updateDisplay();

private:
    QWebEngineWebAuthUxRequest *uxRequest;
    QButtonGroup *buttonGroup = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *selectAccountWidget = nullptr;
    QVBoxLayout *selectAccountLayout = nullptr;

    void setupSelectAccountUI();
    void setupCollectPinUI();
    void setupFinishCollectTokenUI();
    void setupErrorUI();
    void onCancelRequest();
    void onRetry();
    void onAcceptRequest();
    void clearSelectAccountButtons();

    Ui::CDlgWebAuth *ui;
};

