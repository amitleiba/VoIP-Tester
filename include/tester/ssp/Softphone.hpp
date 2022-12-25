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
        _account(sp_a)
    {
        _account.applyAccount();
        if(true) //TODO: need to think how to choose which account will call to whom
        {
            SSPCall *call = new SSPCall(_account);
            _account.setCall(call);
            std::string d_uri = "sip:"+std::to_string(sp_a.id - 1) + sp_a.domain;
            call->callTo(d_uri);
            //TODO: when to stop the call
        }
    }
    ~Softphone();

private:
    SSPAccount _account;

};