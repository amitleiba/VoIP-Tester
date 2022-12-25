#pragma once

#include<iostream>
#include<pjsua2.hpp>

#include"SSPCall.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount();
    ~SSPAccount();
    void createAccount(int account_id, std::string & domain, std::string & password);
    void onRegState(pj::OnRegStateParam &prm) override;    
    void onIncomingCall(pj::OnIncomingCallParam &iprm) override;
    void removeCall(pj::Call *call);
    void pushToCalls(SSPCall * call);
    
private:
    std::vector<pj::Call *> _calls;
};

    SSPAccount::SSPAccount() {}

    SSPAccount::~SSPAccount() 
    {
        shutdown();
        std::cout << "*** Account is being deleted: No of _calls="
            << _calls.size() << std::endl;

        for (std::vector< pj::Call *>::iterator it = _calls.begin();
                it != _calls.end(); )
            {
                delete (*it);
                it = _calls.erase(it);
            }
    }

    void SSPAccount::createAccount(int account_id, std::string & domain, std::string & password)
    {
        pj::AccountConfig acc_cfg;
        std::string uri = "sip:" + std::to_string(account_id) + domain;
        acc_cfg.idUri = uri;
        acc_cfg.regConfig.registrarUri = "sip:" + domain;

        #if PJSIP_HAS_DIGEST_AKA_AUTH
            AuthCredInfo aci("Digest", "*", "test", PJSIP_CRED_DATA_EXT_AKA | PJSIP_CRED_DATA_PLAIN_PASSWD, "passwd");
            aci.akaK = "passwd";
        #else
            pj::AuthCredInfo aci("digest", "*", std::to_string(account_id), 0, password);
        #endif
        
        acc_cfg.sipConfig.authCreds.push_back(aci);
        try{
            create(acc_cfg);
        } catch (...) {
            std::cout << "Adding account failed" << std::endl;
        }
    }

    void SSPAccount::onRegState(pj::OnRegStateParam &prm)
    {
        pj::AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
    }

    void SSPAccount::onIncomingCall(pj::OnIncomingCallParam &iprm)
    {
        pj::Call *call = new SSPCall(*this, iprm.callId);
        pj::CallInfo ci = call->getInfo();
        pj::CallOpParam prm;
        
        std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                  << ci.stateText << "]" << std::endl;
        
        _calls.push_back(call);
        prm.statusCode = (pjsip_status_code)200;
        call->answer(prm);
    }
    
    void SSPAccount::removeCall(pj::Call *call)
    {
        for (std::vector<pj::Call *>::iterator it = _calls.begin();
            it != _calls.end(); ++it)
        {
            if (*it == call) {
                _calls.erase(it);
                break;
            }
        }
    }

    void SSPAccount::pushToCalls(SSPCall * call)
    {
        _calls.push_back(call);
    }