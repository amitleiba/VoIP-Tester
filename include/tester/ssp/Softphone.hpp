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
        _account(args.id, args.domain, args.secret,
        std::bind(&Softphone::onIncomingCall, this, std::placeholders::_1),
        std::bind(&Softphone::onRegState, this, std::placeholders::_1)),
        _call(std::make_shared<SSPCall>(&_account, std::bind(&Softphone::onCallState, this,
            std::placeholders::_1)))
    {
        _uri = SIP + args.id + SEPARATOR + args.domain;
        _account.apply();
    }

    ~Softphone() = default;

    void call(Softphone & sp)
    {
        _call->callTo(sp.getUri());
    }

    void onCallState(const pj::OnCallStateParam &prm)
    {
        auto ci = _call->getInfo();
        PJ_UNUSED_ARG(prm);
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED) 
        {
            clearCall();
        }
    }

    void onIncomingCall(const pj::OnIncomingCallParam &iprm)
    {
        auto incomingCall = std::make_shared<SSPCall>(&_account, std::bind(&Softphone::onCallState,
            this, std::placeholders::_1), iprm.callId);
        if(!isActive())
        {
            _call = std::move(incomingCall);
            pj::CallInfo ci = _call->getInfo();
            pj::CallOpParam prm;
            std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                    << ci.stateText << "]" << std::endl;
            prm.statusCode = (pjsip_status_code)200;
            _call->answer(prm);
        }
        else
        {
            pj::CallOpParam opcode;
            opcode.statusCode = PJSIP_SC_BUSY_HERE;
            incomingCall->hangup(opcode);
        }
    }

    void hangup()
    {
        pj::CallOpParam opcode;
        opcode.statusCode = PJSIP_SC_DECLINE;
        _call->hangup(opcode);
    }

    void onRegState(const pj::OnRegStateParam &prm) 
    {   
        pj::AccountInfo ai = _account.getInfo();
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
    }

    bool isActive()
    {
        return _call->isActive();
    }

    std::string getUri()
    {
        return _uri;
    }

private:
    void clearCall()
    {
        _call = std::make_shared<SSPCall>(&_account, std::bind(&Softphone::onCallState, this,
            std::placeholders::_1));
    }

    static constexpr auto SIP = "sip:";
    static constexpr auto SEPARATOR = "@";

    std::string _uri;
    SSPAccount _account;
    std::shared_ptr<SSPCall> _call;
};