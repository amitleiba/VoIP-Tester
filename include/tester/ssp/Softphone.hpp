#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <memory>

#include "SSPAccount.hpp"
#include "SSPCall.hpp"
#include "SoftphoneArguments.hpp"

#include "../db/StreamLogger.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments & args,
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &, const int)> onCallState,
    std::function<void(const pj::OnRegStateParam &, const pj::AccountInfo &, const int)> onRegState,
    std::function<void(std::shared_ptr<SSPCall> &, std::shared_ptr<SSPCall>, const int)> onIncomingCall):
        _softphoneId(args.id),
        _account(std::make_shared<SSPAccount>(std::to_string(args.id), args.domain, args.secret,
        std::bind(&Softphone::onIncomingCall, this, std::placeholders::_1),
        std::bind(&Softphone::onRegState, this, std::placeholders::_1))),
        // _call(std::make_shared<SSPCall>(&_account, std::bind(&Softphone::onCallState, this,
        //     std::placeholders::_1))),
        _call(nullptr),
        _onCallState(std::move(onCallState)),
        _onRegState(std::move(onRegState)),
        _onIncomingCall(std::move(onIncomingCall)),
        _answeredIncomingCall(false),
        _declinedIncomingCall(false)
    {
        _uri = SIP + std::to_string(args.id) + SEPARATOR + args.domain;

        _account->apply();
    }

    ~Softphone() = default;

    void call(const Softphone &sp)
    {
        call(sp.getUri());
    }

    void call(const std::string & destUri)
    {
        if(!_call)
        {
            _call = std::make_shared<SSPCall>(&(*_account), std::bind(&Softphone::onCallState, this,
                    std::placeholders::_1));
            
        }
        if(isRegistered() && !isActive())
        {
            _call->callTo(destUri);
        }
    }

    void onCallState(const pj::OnCallStateParam &prm)
    {
        pj::CallInfo ci = _call->getInfo();
        _onCallState(prm, ci, _softphoneId);
    }

    pjsip_inv_state getState()
    {
        return _call->getInfo().state;
    }

    void onIncomingCall(const pj::OnIncomingCallParam &iprm)
    {
        std::cout << "Main OnIncomingCall" << std::endl;
        // auto incomingCall = std::make_shared<SSPCall>(&_account, std::bind(&Softphone::onCallState,
        //     this, std::placeholders::_1), iprm.callId);
        auto newCall = std::make_shared<SSPCall>(&(*_account), std::bind(&Softphone::onCallState, this,
                std::placeholders::_1), iprm.callId);

        
        _onIncomingCall(_call, newCall, _softphoneId);
    }

    // void answer()
    // {
    //     pj::CallOpParam prm;
    //     prm.statusCode = PJSIP_SC_OK;
    //     _call->answer(prm);
    // }

    void hangup()
    {
        if(isActive())
        {
            pj::CallOpParam opcode;
            opcode.statusCode = PJSIP_SC_DECLINE;
            _call->hangup(opcode);
        }
    }

    void setAnsweredIncomingCall(bool answer)
    {
        _answeredIncomingCall =  answer;
    }

    void setDeclinedIncomingCall(bool decline)
    {
        _declinedIncomingCall = decline;
    }

    bool getAnsweredIncomingCall()
    {
        return _answeredIncomingCall;
    }

    bool getDeclinedIncomingCall()
    {
        return _declinedIncomingCall;
    }

    void onRegState(const pj::OnRegStateParam &prm) 
    {   
        pj::AccountInfo ai = _account->getInfo();
        _onRegState(prm, ai, _softphoneId);
    }

    bool isActive()
    {
        return _call && _call->isActive();
    }

    int getId() const
    {
        return _softphoneId;
    }

    std::string getUri() const
    {
        return _uri;
    }

    bool isRegistered()
    {
        return _account->getInfo().regIsActive;
    }

    void clearCall()
    {
        _call = nullptr;
    }
    
private:
    static constexpr auto SIP = "sip:";
    static constexpr auto SEPARATOR = "@";

    std::string _uri;
    const int _softphoneId;
    std::shared_ptr<SSPAccount> _account;
    std::shared_ptr<SSPCall> _call;
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &, const int)> _onCallState;
    std::function<void(const pj::OnRegStateParam &, const pj::AccountInfo &, const int)> _onRegState;
    std::function<void(std::shared_ptr<SSPCall> &, std::shared_ptr<SSPCall>, const int)> _onIncomingCall;

    bool _answeredIncomingCall;
    bool _declinedIncomingCall;
};