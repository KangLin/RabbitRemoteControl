#ifndef CPARAMETERLIBVNCSERVER_H
#define CPARAMETERLIBVNCSERVER_H

#include <ParameterBase.h>

class CParameterLibVNCServer : public CParameterBase
{
    Q_OBJECT
public:
    explicit CParameterLibVNCServer(QObject *parent = nullptr);
    
    // CParameterConnecter interface
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;
        
    enum class emVncProxy {
        UltraVncRepeater = (int) emProxy::User + 1
    };
    bool GetShared() const;
    void SetShared(bool newShared);
    bool GetEnableCompressLevel() const;
    void SetEnableCompressLevel(bool newCompressLevel);
    int GetCompressLevel() const;
    void SetCompressLevel(int newCompressLevel);
    bool GetJpeg() const;
    void SetJpeg(bool newJpeg);
    int GetQualityLevel() const;
    void SetQualityLevel(int newQualityLevel);
    
private:
    bool m_bShared;
    bool m_bCompressLevel;
    int m_nCompressLevel;
    bool m_bJpeg;
    int m_bQualityLevel;
};

#endif // CPARAMETERLIBVNCSERVER_H
