#pragma once

#include<iostream>
#include<string>

#include"SSPAccount.hpp"
#include"SSPCall.hpp"
#include"SoftphoneArguments.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments & sp_a):
        _account(sp_a.id, sp_a.domain, sp_a.secret)
    {
        _account.applyAccount();
    }
    ~Softphone();

    void makeNewCall()
    {  
        SSPCall *call = new SSPCall(_account);
        _account.setCall(call);
            std::string d_uri = "sip: next account id domain";
            call->callTo(d_uri);
            //TODO: when to stop the call
    }

private:
    SSPAccount _account;
};