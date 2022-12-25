#pragma once

#include<iostream>
#include<pjsua2.hpp>

#include"Softphone.hpp"
#include"SoftphoneArguments.hpp"

class SoftphoneManager
{
public:
    SoftphoneManager(int, int, std::string &);
    ~SoftphoneManager();
private:
    const int _PORT;
    std::string & _PBX_IP;
    pj::Endpoint _endpoint;
    std::vector<Softphone *> _softphones;

};

SoftphoneManager::SoftphoneManager(int number_of_softphones, int port, std::string & PBX_IP):
    _PORT(port), _PBX_IP(PBX_IP)
{
    try
    {
        //Create library
        _endpoint.libCreate();

        // Init library
        pj::EpConfig ep_cfg;
        ep_cfg.logConfig.level = 1;
        _endpoint.libInit( ep_cfg );

        // Transport
        pj::TransportConfig tcfg;
        tcfg.port = _PORT;
        _endpoint.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

        // Start library
        _endpoint.libStart();
        std::cout << "*** PJSUA2 STARTED ***" << std::endl;
    }
    catch(pj::Error & err)
    {
        std::cout << "Exception: " << err.info() << std::endl;
    }
    
    SoftphoneArguments sp_a;
    sp_a.domain = _PBX_IP;
    sp_a.secret = "12345678";
    sp_a.timeout = 5000;
    sp_a.id = 1000;

    for(int id = 0; id < (number_of_softphones * 2); id++)
    {
        Softphone sp(sp_a);
        sp_a.id++;
        _softphones.push_back(&sp);
    }
}

