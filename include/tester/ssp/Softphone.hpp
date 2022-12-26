#pragma once

#include<iostream>
#include<string>
#include<functional>

#include"SSPAccount.hpp"
#include"SSPCall.hpp"
#include"SoftphoneArguments.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments & args):
        _account(args.id, args.domain, args.secret, std::bind(&inComingCall, *this, std::placeholders::_1)),
        _call(&_account, std::bind(&onCallState, *this, std::placeholders::_2)) //TODO
    {
        _account.applyAccount();
    }

    ~Softphone();

    void call(std::string & id_to_call, std::string & domain)
    {  
        _call = new SSPCall(*this); //TODO
        std::string d_uri = SIP + id_to_call + domain;
        _call->callTo(d_uri);
    }

    void onCallState(pj::CallInfo ci, const pj::OnCallStateParam &prm)
    {
        PJ_UNUSED_ARG(prm);
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
            /* Delete the call */
            delete _call;
        }
    }

    void inComingCall(const pj::OnIncomingCallParam &iprm)
    {
        SSPCall *in_call = new SSPCall(&_account, iprm.callId); //TODO
        if(!_call->isActive()){
            _call = std::move(in_call);
            pj::CallInfo ci = _call->getInfo();
            pj::CallOpParam prm;
            std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                    << ci.stateText << "]" << std::endl;
            prm.statusCode = (pjsip_status_code)200;
            _call->answer(prm);
        }
        else{
            in_call->hangup(PJSIP_SC_BUSY_HERE);
            delete in_call;
        }
    }

    bool hasActiveCall()
    {
        pj::CallInfo callInfo = _call->getInfo();
        if(callInfo.state == PJSIP_INV_STATE_CONFIRMED)
            return true;
        return false;
    }

private:
    static constexpr auto SIP = "sip:";

    SSPAccount _account;
    SSPCall *_call;
};