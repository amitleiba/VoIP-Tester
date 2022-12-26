#pragma once

#include<iostream>
#include<string>

#include<pjsua2.hpp>

#include"SSPCall.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount(const std::string & id, const std::string & domain, const std::string & secret) 
    {
        std::string uri = sip + id + domain;
        _acc_cfg.idUri = uri;
        _acc_cfg.regConfig.registrarUri = sip + domain;

        pj::AuthCredInfo aci(scheme, realm, id, data_type, secret);

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
            std::cerr << "Adding account failed" << std::endl;
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

    void call(std::string & id_to_call, std::string & domain)
    {
        _call = new SSPCall(*this);
        std::string d_uri = "sip:" + id_to_call + "" + domain;
        _call->callTo(d_uri);
        //TODO: when to stop the call
    }
    
private:
    SSPCall * _call;
    pj::AccountConfig _acc_cfg;

    static constexpr auto sip = "sip:";
    static constexpr auto scheme = "digest";
    static constexpr auto realm = "x";
    static constexpr int data_type = 0;
};