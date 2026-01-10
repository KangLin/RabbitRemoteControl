#pragma once
#include <QSize>
#include "Parameter.h"

class CParameterHistory : public CParameter
{
    Q_OBJECT
public:
    explicit CParameterHistory(QObject *parent = nullptr, const QString& szPrefix = QString());

public:
    int GetLimit() const;
    void SetLimit(int newLimit);
private:
    int m_Limit;

public:
    QSize GetWindowSize() const;
    void SetWindowSize(const QSize &newWindowSize);
private:
    QSize m_WindowSize;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

