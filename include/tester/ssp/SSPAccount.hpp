#pragma once

#include<iostream>
#include<pjsua2.hpp>

#include"SSPCall.hpp"

class SSPAccount : public pj::Account
{
public:
    SSPAccount() {}
    ~SSPAccount() 
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

private:
    std::vector<pj::Call *> _calls;

    void removeCall(pj::Call *call)
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

    void onRegState(pj::OnRegStateParam &prm) override
    {
        pj::AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
    }

    void onIncomingCall(pj::OnIncomingCallParam &iprm) override
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


    

};