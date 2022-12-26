#pragma once

#include<iostream>
#include<string>

#include<pjsua2.hpp>

#include"SSPCall.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount(const std::string & id, const std::string & domain, const std::string & secret,
        std::function<void(const pj::OnIncomingCallParam &iprm)> onIncomingCall):
        _onIncomingCall(onIncomingCall)
    {
        std::string uri = SIP + id + domain;
        _acc_cfg.idUri = uri;
        _acc_cfg.regConfig.registrarUri = SIP + domain;

        pj::AuthCredInfo aci(SCHEME, REALM, id, DATA_TYPE, secret);

        _acc_cfg.sipConfig.authCreds.push_back(aci);
    }

    ~SSPAccount()
    {
        shutdown();
        std::cout << "*** Account is being deleted ***" << std::endl;
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
        _onIncomingCall(iprm);
    }
    
private:
    static constexpr auto SIP = "sip:";
    static constexpr auto SCHEME = "digest";
    static constexpr auto REALM = "x";
    static constexpr int DATA_TYPE = 0;

    pj::AccountConfig _acc_cfg;

    std::function<void(const pj::OnIncomingCallParam &iprm)> _onIncomingCall;
    
};