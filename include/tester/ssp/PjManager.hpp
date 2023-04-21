#pragma once

#include <iostream>

#include <pjsua2.hpp>

class PjManager
{
public:
    PjManager(int port, int logLevel)
    {
        pjLibraryInit(port, logLevel);
    }

    ~PjManager()
    {
        _endpoint.libDestroy(); //todo
    }

private:
    void pjLibraryInit(int port, int logLevel)
    {
        try
        {
            //Create library
            _endpoint.libCreate();

            // Init library
            pj::EpConfig ep_cfg;
            ep_cfg.uaConfig.threadCnt = 2;
            std::cout << ep_cfg.uaConfig.mainThreadOnly <<std::endl;
            std::cout << ep_cfg.uaConfig.threadCnt <<std::endl;
            ep_cfg.logConfig.level = logLevel;
            _endpoint.libInit( ep_cfg );

            // Transport
            pj::TransportConfig tcfg;
            tcfg.port = port;
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

    pj::Endpoint _endpoint;

};