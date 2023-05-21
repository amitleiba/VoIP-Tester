#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "SSPAccount.hpp"
#include "SSPCall.hpp"
#include "SoftphoneArguments.hpp"
#include "../db/StreamLogger.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments & args,
    std::function<void(const pj::CallInfo&, pj::Call *, Softphone&)> onIncomingCall,
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &, Softphone&)> onCallState,
    std::function<void(const pj::OnRegStateParam &, const pj::AccountInfo &, Softphone&)> onRegState):
        _softphoneId(args.id),
        _account(std::to_string(args.id), args.domain, args.secret,
            std::bind(&Softphone::onRegState, this, std::placeholders::_1),
            std::bind(&Softphone::onCallState, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&Softphone::onIncomingCall, this, std::placeholders::_1, std::placeholders::_2)),
        _answeredIncomingCall(false),
        _declinedIncomingCall(false),
        _active(true),
        _onIncomingCall(onIncomingCall),
        _onCallState(onCallState),
        _onRegState(onRegState)
    {
        _uri = SIP + std::to_string(args.id) + SEPARATOR + args.domain;

        _account.apply();
    }

    ~Softphone()
    {
        _active = false;
    }

    void onIncomingCall(const pj::CallInfo& callInfo, pj::Call * incomingCall)
    {
        _onIncomingCall(callInfo, incomingCall, *this);
    }

    void onCallState(const pj::OnCallStateParam &prm, const pj::CallInfo &ci)
    {
        // PJ_UNUSED_ARG(prm);
        // LOG_INFO << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        //     << "]" << std::endl;
        // std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        //     << "]" << std::endl;
        _onCallState(prm, ci, *this);
    }

    void onRegState(const pj::OnRegStateParam &prm) 
    {   
        pj::AccountInfo ai = _account.getInfo();
        _onRegState(prm, ai, *this);
        
        // LOG_INFO << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
        //     << prm.code << std::endl;  
        // std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
        //     << prm.code << std::endl;
    }

    void call(const Softphone &sp)
    {
        call(sp.getUri());
    }

    void call(const std::string & destUri)
    {
        _account.call(destUri);
    }

    void answer(const pj::CallOpParam& callOpParam)
    {
        _account.answer(callOpParam);
    }

    void hangup()
    {
        _account.hangup();
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

    void unregister()
    {
        _account.setRegistration(false);
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
        return _account.getInfo().regIsActive;
    }

    void setCall(pj::Call* newCall)
    {
        _account.setCall(newCall);
    }

    bool activeCall()
    {
        return _account.activeCall();
    }
    
private:
    static constexpr auto SIP = "sip:";
    static constexpr auto SEPARATOR = "@";
    std::string _uri;
    const int _softphoneId;
    SSPAccount _account;
    bool _answeredIncomingCall;
    bool _declinedIncomingCall;
    bool _active;

    std::function<void(const pj::CallInfo&, pj::Call *, Softphone&)> _onIncomingCall;
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &, Softphone&)> _onCallState;
    std::function<void(const pj::OnRegStateParam &, const pj::AccountInfo &, Softphone&)> _onRegState;
};