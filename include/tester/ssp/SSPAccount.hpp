#pragma once

#include <iostream>
#include <string>

#include <pjsua2.hpp>

#include "SSPCall.hpp"

#include "../db/StreamLogger.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount(const std::string & id, const std::string & domain, const std::string & secret,
        std::function<void(const pj::OnIncomingCallParam &)> onIncomingCall,
        std::function<void(const pj::OnRegStateParam &)> onRegState):
        _onIncomingCall(std::move(onIncomingCall)), _onRegState(std::move(onRegState))
    {
        std::string uri = SIP + id + SEPARATOR + domain;
        _config.idUri = uri;
        _config.regConfig.registrarUri = SIP + domain;

        pj::AuthCredInfo aci(SCHEME, REALM, id, DATA_TYPE, secret);

        _config.sipConfig.authCreds.push_back(aci);
    }

    ~SSPAccount()
    {
        shutdown();
        // LOG_INFO  << "*** Account " + std::to_string(getId()) +" is being deleted ***" << std::endl;
        std::cout << "*** Account " + std::to_string(getId()) +" is being deleted ***" << std::endl;        
    }

    void apply()
    {
        try{
            create(_config);
        } catch (...) {
            // LOG_ERORR << "Adding account failed" << std::endl;
            std::cerr << "Adding account failed" << std::endl;
        }
    }

    void onRegState(pj::OnRegStateParam &prm) override 
    {   
        _onRegState(prm);
    }

    virtual void onIncomingCall(pj::OnIncomingCallParam &iprm) override
    {
        _onIncomingCall(iprm);
    }
    
private:
    static constexpr auto SIP = "sip:";
    static constexpr auto SEPARATOR = "@";
    static constexpr auto SCHEME = "digest";
    static constexpr auto REALM = "*";
    static constexpr int DATA_TYPE = 0;

    pj::AccountConfig _config;

    std::function<void(const pj::OnIncomingCallParam &)> _onIncomingCall;
    std::function<void(const pj::OnRegStateParam &)> _onRegState;
    
};