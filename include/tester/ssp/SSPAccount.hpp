#pragma once

#include<iostream>
#include<string>

#include<pjsua2.hpp>

#include"SSPCall.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount(const std::string & id, const std::string & domain, const std::string & secret,
        std::function<void(const pj::OnIncomingCallParam &)> onIncomingCall):
        _onIncomingCall(std::move(onIncomingCall))
    {
        std::string uri = SIP + id + AT + domain;
        _config.idUri = uri;
        _config.regConfig.registrarUri = SIP + domain;

        pj::AuthCredInfo aci(SCHEME, REALM, id, DATA_TYPE, secret);

        _config.sipConfig.authCreds.push_back(aci);
    }

    ~SSPAccount()
    {
        std::cout << "*** Account " + std::to_string(getId()) +" is being deleted ***" << std::endl;
        shutdown();
        
    }

    void apply()
    {
        try{
            create(_config);
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
    static constexpr auto AT = "@";
    static constexpr auto SCHEME = "digest";
    static constexpr auto REALM = "*";
    static constexpr int DATA_TYPE = 0;

    pj::AccountConfig _config;

    std::function<void(const pj::OnIncomingCallParam &)> _onIncomingCall;
    
};