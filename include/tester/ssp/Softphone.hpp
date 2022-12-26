#pragma once

#include<iostream>
#include<string>

#include"SSPAccount.hpp"
#include"SSPCall.hpp"
#include"SoftphoneArguments.hpp"

class Softphone
{
public:
    Softphone(const SoftphoneArguments sp_a):
        _account(sp_a.id, sp_a.domain, sp_a.secret)
    {
        _account.applyAccount();
    }
    ~Softphone();

    void call(std::string & id_to_call, std::string & domain)
    {  
        _account.call(id_to_call, domain);
    }

private:
    SSPAccount _account;
};