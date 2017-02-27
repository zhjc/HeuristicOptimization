
#ifndef P_CENTER_PARAMETER_H
#define P_CENTER_PARAMETER_H

#include "common/ho_parameter.h"

HO_NAMESPACE_BEGIN(utility)

class PCenterConfigHandler : public HoConfigHandler
{
public:
    PCenterConfigHandler(const std::string& conffile);

    ~PCenterConfigHandler(){}

public:
    
    bool IsTabuFVPair()
    {
        return m_bTabuFVPair;
    }

    bool IsTabuFOnly()
    {
        return m_bTabuFOnly;
    }

    bool IsTabuVOnly()
    {
        return m_bTabuVOnly;
    }

    int GetAddLengthOfTabu() 
    {
        return m_nAddLengthOfTabu;
    }

    double GetMutationProb()
    {
        return m_dMutationProb;
    }

    void SetMutationProb(double dMutation)
    {
        m_dMutationProb = dMutation;
    }

    double GetCrossoverProb()
    {
        return m_dCrossoverProb;
    }

    void SetCrossoverProb(double dCross)
    {
        m_dCrossoverProb = dCross;
    }
    
    hoStatus ParseConfigFile(const std::string& strConfFile);

private:

    // other config
    bool m_bTabuFVPair;
    bool m_bTabuFOnly;
    bool m_bTabuVOnly;

    int m_nAddLengthOfTabu;

    double m_dMutationProb;
    double m_dCrossoverProb;
};

HO_NAMESPACE_END

#endif
