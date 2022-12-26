#pragma once

#include<iostream>
#include<pjsua2.hpp>
#include<unordered_map>

#include"Softphone.hpp"
#include"SoftphoneArguments.hpp"

class SoftphoneManager
{
public:
    SoftphoneManager(int number_of_softphones, int port, std::string & PBX_IP):
        _PORT(port), _PBX_IP(PBX_IP)
    {
        pjLibraryInit();
        createSoftphone(number_of_softphones);
    }

    ~SoftphoneManager() = default;

    void pjLibraryInit()
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
    }

    void createSoftphone(int number_of_softphones)
    {
        SoftphoneArguments sp_a;
        sp_a.domain = _PBX_IP;
        sp_a.secret = "12345678";
        sp_a.timeout = 5000;
        //sp_a.id = 1000;

        for(int i = 0; i < (number_of_softphones * 2); i++)
        {
            sp_a.id = std::to_string(i + 1000);
            Softphone sp(sp_a);
            _softphone_map.insert({sp_a.id, &sp});
        }
        //TODO: think where to use sp.makeCall()
    }

private:
    const int _PORT;
    std::string & _PBX_IP;
    pj::Endpoint _endpoint;
    std::unordered_map<std::string, Softphone *> _softphone_map;

};
