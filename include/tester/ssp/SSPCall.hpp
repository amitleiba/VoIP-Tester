#pragma once

#include <iostream>
#include <functional>

#include <pjsua2.hpp>

#include "../db/StreamLogger.hpp"

class SSPAccount;

class SSPCall : public pj::Call
{
public:
    SSPCall(pj::Account &acc, std::function<void(const pj::OnCallStateParam & )> onCallState, int call_id = PJSUA_INVALID_ID);

    ~SSPCall();

    void onCallState(pj::OnCallStateParam &prm) override;

    void onCallMediaState(pj::OnCallMediaStateParam &prm) override;
    
private:
    std::function<void(const pj::OnCallStateParam &)> _onCallState;
};