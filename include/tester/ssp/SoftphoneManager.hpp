#pragma once

#include<iostream>
#include<pjsua2.hpp>
#include<unordered_map>
#include <string>

#include <pjsua-lib/pjsua.h>

#include"Softphone.hpp"
#include"SoftphoneArguments.hpp"

class SoftphoneManager
{
public:
    SoftphoneManager(int port, std::string domain):
        _port(port), _domain(std::move(domain))
    {
    }

    ~SoftphoneManager()
    {
        _endpoint.libDestroy();
    }

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

    void runSpamTest(int amount)
    {
        registerSoftphones(amount * 2);
        for(int i = 0; i < (amount * 2); i += 2)
        {
            if(_softphones.at(std::to_string(i + START_URI))->isAccountValid())
            {
                _softphones.at(std::to_string(i + START_URI))->call(*_softphones.at(std::to_string(i + 1 + START_URI)));
            }
        }
        pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        for(int i = 0; i < (amount * 2); i += 2)
        {
            _softphones.at(std::to_string(i + START_URI))->hangup();
        }
    }

private:
    void registerSoftphones(int amount)
    {
        SoftphoneArguments args;
        args.domain = _domain;
        args.secret = "12345678";
        args.timeout = 5000;

        for(int i = 0; i < amount; i++)
        {
            args.id = std::to_string(i + START_URI);
            Softphone *sp = new Softphone(args);
            _softphones.insert({args.id, sp});
            pj_thread_sleep(MILLISECONDS_TO_SECONDS);
        }
    }

    static constexpr int MILLISECONDS_TO_SECONDS = 1000;
    static constexpr int TEST_SLEEP_DURATION = 10;
    static constexpr int START_URI = 1000;

    const int _port;
    const std::string _domain;
    pj::Endpoint _endpoint;
    std::unordered_map<std::string, Softphone*> _softphones;

};
