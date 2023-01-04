#pragma once

#include<iostream>
#include<unordered_map>
#include<string>
#include<memory>

#include<pjsua2.hpp>

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

    void pjLibraryInit(int logLevel)
    {
        try
        {
            //Create library
            _endpoint.libCreate();

            // Init library
            pj::EpConfig ep_cfg;
            ep_cfg.logConfig.level = logLevel;
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
            _softphones.at(i)->call(*_softphones.at(i + 1));
            pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        }
        pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        for(int i = 0; i < (amount * 2); i += 2)
        {
            _softphones.at(i)->hangup();
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
            _softphones.emplace_back(std::make_shared<Softphone>(args));
            pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        }
    }

    static constexpr int MILLISECONDS_TO_SECONDS = 1000;
    static constexpr int TEST_SLEEP_DURATION = 1;
    static constexpr int START_URI = 1000;

    const int _port;
    const std::string _domain;
    pj::Endpoint _endpoint;
    std::vector<std::shared_ptr<Softphone>> _softphones;
};
