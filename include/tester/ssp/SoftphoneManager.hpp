#pragma once

#include<iostream>
#include<pjsua2.hpp>
#include<unordered_map>
#include <string>

#include"Softphone.hpp"
#include"SoftphoneArguments.hpp"

class SoftphoneManager
{
public:
    SoftphoneManager(int number_of_softphones, int port, std::string domain):
        _port(port), _domain(std::move(domain))
    {
        pjLibraryInit();
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
            tcfg.port = _port;
            _endpoint.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

            // Start library
            _endpoint.libStart();
            std::cout << "*** PJSUA2 STARTED ***" << std::endl;
        }
        catch(pj::Error & err)
        {
            std::cerr << "Exception: " << err.info() << std::endl;
        }
    }

    void createSoftphone(int number_of_softphones)
    {
        SoftphoneArguments args;
        args.domain = _domain;
        args.secret = "12345678";
        args.timeout = 5000;
        //args.id = 1000;

        for(int i = 0; i < (number_of_softphones * 2); i++)
        {
            args.id = std::to_string(i + 1000);
            Softphone sp(args);
            _softphones.insert({args.id, std::move(sp)});
        }
        //TODO: think where to use sp.makeCall()
    }

private:
    const int _port;
    const std::string _domain;
    pj::Endpoint _endpoint;
    std::unordered_map<std::string, Softphone> _softphones;

};
