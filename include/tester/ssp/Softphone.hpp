#pragma once

#include<iostream>
#include<string>

#include"SSPAccount.hpp"
#include"SSPCall.hpp"
#include"SoftphoneArguments.hpp"

class Softphone
{
public:
    Softphone(SoftphoneArguments sp_a)
    {
        _account.createAccount(sp_a.id, sp_a.domain, sp_a.secret);
        if(sp_a.id % 2 == 1)
        {
            SSPCall *call = new SSPCall(_account);
            _account.pushToCalls(call);
            std::string d_uri = "sip:"+std::to_string(sp_a.id - 1) + sp_a.domain;
            call->callTo(d_uri);
            //TODO: when to stop the call
        }
    }
    ~Softphone();

private:
    SSPAccount _account;

};