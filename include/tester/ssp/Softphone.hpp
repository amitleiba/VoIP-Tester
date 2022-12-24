#pragma once

#include"SSPAccount.hpp"
#include"SSPCall.hpp"

class Softphone
{
public:
    Softphone(/* args */);
    ~Softphone();

private:
    std::vector<SSPAccount *> _accounts;

};
