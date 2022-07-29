#ifndef CParameterRabbitVnc_H
#define CParameterRabbitVnc_H

#include "ParameterConnecter.h"

class CParameterRabbitVNC : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterRabbitVNC(QObject *parent = nullptr);
    
public:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
    
    /*!
     * \brief Check whether the parameters are complete
     *  to decide whether to open the parameter dialog 
     * \return 
     */
    virtual bool GetCheckCompleted() override;
    
    enum COLOR_LEVEL {
        Full,
        Medium,
        Low,
        VeryLow
    };

    bool GetShared() const;
    void SetShared(bool newShared);
    
    bool GetBufferEndRefresh() const;
    void SetBufferEndRefresh(bool newBufferEndRefresh);
    
    bool GetSupportsDesktopResize() const;
    void SetSupportsDesktopResize(bool newSupportsDesktopResize);
    
    bool GetAutoSelect() const;
    void SetAutoSelect(bool newAutoSelect);
    
    COLOR_LEVEL GetColorLevel() const;
    void SetColorLevel(COLOR_LEVEL newColorLevel);
    
    int GetEncoding() const;
    void SetEncoding(int newEncoding);
    
    bool GetEnableCompressLevel() const;
    void SetEnableCompressLevel(bool newCompressLevel);
    
    int GetCompressLevel() const;
    void SetCompressLevel(int newCompressLevel);
    
    bool GetNoJpeg() const;
    void SetNoJpeg(bool newNoJpeg);
    
    int GetQualityLevel() const;
    void SetQualityLevel(int newQualityLevel);
    
    bool GetIce() const;
    void SetIce(bool newIce);
    
    const QString &GetPeerUser() const;
    void SetPeerUser(const QString &newPeerUser);

private:
    QString szServerName;

    bool m_bShared;
    bool m_bBufferEndRefresh;
    bool m_bSupportsDesktopResize;

    bool m_bAutoSelect;
    COLOR_LEVEL m_nColorLevel;
    int m_nEncoding;
    bool m_bCompressLevel;
    int m_nCompressLevel;
    bool m_bNoJpeg;
    int m_nQualityLevel;
    
    bool m_bIce;
    QString m_szPeerUser;

};

#endif // CParameterRabbitVnc_H
