#include<iostream>
#include<string>

#include"../include/tester/ssp/Softphone.hpp"
#include"../include/tester/ssp/SoftphoneArguments.hpp"


void pjLibraryInit(pj::Endpoint * _endpoint)
{
    try
    {
        //Create library
        _endpoint->libCreate();

        // Init library
        pj::EpConfig ep_cfg;
        ep_cfg.logConfig.level = 5;
        _endpoint->libInit( ep_cfg );

        // Transport
        pj::TransportConfig tcfg;
        tcfg.port = 6061;
        _endpoint->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

        // Start library
        _endpoint->libStart();
        std::cout << "*** PJSUA2 STARTED ***" << std::endl;
    }
    catch(pj::Error & err)
    {
        std::cerr << "Exception: " << err.info() << std::endl;
    }
}

int main(){
    pj::Endpoint _endpoint;

    pjLibraryInit(&_endpoint);

    std::string DOMAIN = "192.168.1.46";

    SoftphoneArguments args;
    args.domain = DOMAIN;
    args.secret = "12345678";
    args.timeout = 5000;

    args.id = "1000";
    Softphone sp1(args);

    args.id = "1001";
    Softphone sp2(args);

    args.id = "1002";
    Softphone sp3(args);

    std::string uri_sp1 = "sip:1000@" + args.domain;
    std::string uri_sp2 = "sip:1006@" + args.domain;

    sp1.call(uri_sp2);

    pj_thread_sleep(5000);

    sp3.call(uri_sp1);

    pj_thread_sleep(100000);

    //_endpoint.libDestroy();
}