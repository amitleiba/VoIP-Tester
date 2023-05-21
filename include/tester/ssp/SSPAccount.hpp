#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include <pjsua2.hpp>

#include "../db/StreamLogger.hpp"

class SSPCall;

class SSPAccount : public pj::Account
{
public:
    SSPAccount(const std::string &id, const std::string &domain, const std::string &secret,
                std::function<void(const pj::OnRegStateParam &)> onRegState,
                std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &)> onCallState,
                std::function<void(const pj::CallInfo&, pj::Call *)> onIncomingCall);

    ~SSPAccount();

    void apply();

    void onRegState(pj::OnRegStateParam &prm) override;

    void onIncomingCall(pj::OnIncomingCallParam &iprm) override;

    void onCallState(const pj::OnCallStateParam &prm);

    void call(const std::string &destUri);

    void hangup();

    void setCall(pj::Call *);

    bool activeCall();

    void answer(const pj::CallOpParam& callOpParam);

private:
    static constexpr auto SIP = "sip:";
    static constexpr auto SEPARATOR = "@";
    static constexpr auto SCHEME = "digest";
    static constexpr auto REALM = "*";
    static constexpr int DATA_TYPE = 0;

    pj::AccountConfig _config;

    std::function<void(const pj::OnRegStateParam &)> _onRegState;
    std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &)> _onCallState;
    std::function<void(const pj::CallInfo&, pj::Call *)> _onIncomingCall;

    pj::Call *_call;
};