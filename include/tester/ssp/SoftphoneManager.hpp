#pragma once

#include<iostream>
#include<pjsua2.hpp>

#include"Softphone.hpp"

class SoftphoneManager
{
public:
    SoftphoneManager();
    ~SoftphoneManager();
private:
    std::vector<Softphone *> _softphones;

};
