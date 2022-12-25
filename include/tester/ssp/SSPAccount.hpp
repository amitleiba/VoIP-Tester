#pragma once

#include<iostream>
#include<pjsua2.hpp>

#include"SSPCall.hpp"
#include"SoftphoneArguments.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount(SoftphoneArguments sp_a) 
    {
        std::string uri = "sip:" + sp_a.id + sp_a.domain;
        _acc_cfg.idUri = uri;
        _acc_cfg.regConfig.registrarUri = "sip:" + sp_a.domain;


        pj::AuthCredInfo aci("digest", "*", sp_a.id, 0, sp_a.secret);

        
        _acc_cfg.sipConfig.authCreds.push_back(aci);
    }

    ~SSPAccount()
    {
        shutdown();
        std::cout << "*** Account is being deleted: No of _calls="
            << (hasActiveCall()? "1" : "0") << std::endl;

        removeCall();
    }

    void applyAccount()
    {
        try{
            create(_acc_cfg);
        } catch (...) {
            std::cout << "Adding account failed" << std::endl;
        }
    }

    void onRegState(pj::OnRegStateParam &prm) override 
    {
        pj::AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
    }

    void onIncomingCall(pj::OnIncomingCallParam &iprm) override
    {
        if(!hasActiveCall())
        {
            _call = new SSPCall(*this, iprm.callId);
            pj::CallInfo ci = _call->getInfo();
            pj::CallOpParam prm;
            
            std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                    << ci.stateText << "]" << std::endl;
            
            prm.statusCode = (pjsip_status_code)200;
            _call->answer(prm);
        }
    }

    void removeCall()
    {
        delete _call;
    }
    
    bool hasActiveCall()
    {
        pj::CallInfo callInfo = _call->getInfo();
        if(callInfo.state == PJSIP_INV_STATE_CONFIRMED)
            return true;
        return false;
    }

    void setCall(SSPCall * call)
    {
        _call = call;
    }
    

private:
    SSPCall * _call;
    pj::AccountConfig _acc_cfg;
};