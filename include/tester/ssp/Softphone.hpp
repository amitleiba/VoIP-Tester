#pragma once

#include<iostream>
#include<string>

#include"SSPAccount.hpp"
#include"SSPCall.hpp"

class Softphone
{
public:
    Softphone(int account_id, std::string & pbx_ip);
    ~Softphone();

private:
    SSPAccount _account;

};
 
Softphone::Softphone(int account_id, std::string & pbx_ip)
{
    _account.createAccount(account_id, pbx_ip);
    if(account_id % 2 == 0){
        SSPCall *call = new SSPCall(_account);
        _account.pushToCalls(call);
        std::string d_uri = "sip:"+std::to_string(account_id - 1) + pbx_ip;
        call->callTo(d_uri);
        //TODO: when to stop the call
    }
}