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
        _account(args.id, args.domain, args.secret, std::bind(&Softphone::onInComingCall, this, std::placeholders::_1))
    {
        _call = new SSPCall(&_account, boundOnCallState);
        _account.applyAccount();
    }

    ~Softphone();

    void call(const std::string & id_to_call, const std::string & domain)
    {
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

    void onInComingCall(const pj::OnIncomingCallParam &iprm)
    {
        SSPCall *in_call = new SSPCall(&_account,boundOnCallState, iprm.callId); //TODO
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
    std::function<void(pj::CallInfo ci, const pj::OnCallStateParam &prm)> boundOnCallState =
        std::bind(&Softphone::onCallState, this, std::placeholders::_1, std::placeholders::_2);
    SSPAccount _account;
    SSPCall *_call;
};