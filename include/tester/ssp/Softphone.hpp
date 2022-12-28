#pragma once

#include<iostream>
#include<string>
#include<functional>
#include<memory>

#include"SSPAccount.hpp"
#include"SSPCall.hpp"
#include"SoftphoneArguments.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments & args):
        _account(args.id, args.domain, args.secret, std::bind(&Softphone::onIncomingCall, this, std::placeholders::_1))
    {
        _call = std::make_unique<SSPCall>(&_account, std::bind(&Softphone::onCallState, this,
            std::placeholders::_1, std::placeholders::_2));
        _account.apply();
    }

    ~Softphone() = default;

    void call(const std::string & uri)
    {
        _call->callTo(uri);
    }

    void onCallState(pj::CallInfo ci, const pj::OnCallStateParam &prm)
    {
        PJ_UNUSED_ARG(prm);
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
            /* Delete the call */
            _call.reset();
        }
    }

    void onIncomingCall(const pj::OnIncomingCallParam &iprm)
    {
        SSPCall *in_call = new SSPCall(&_account, std::bind(&Softphone::onCallState,
            this, std::placeholders::_1, std::placeholders::_2), iprm.callId);
        if(!_call->isActive()){
            _call = std::make_unique<SSPCall>(std::move(in_call));
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
    std::unique_ptr<SSPCall> _call;
};