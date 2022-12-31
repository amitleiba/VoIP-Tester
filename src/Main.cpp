#include<iostream>
#include<string>

#include"../include/tester/ssp/SoftphoneManager.hpp"
#include"../include/tester/ssp/SoftphoneArguments.hpp"


// void pjLibraryInit(pj::Endpoint * _endpoint)
// {
//     try
//     {
//         //Create library
//         _endpoint->libCreate();

//         // Init library
//         pj::EpConfig ep_cfg;
//         ep_cfg.logConfig.level = 2;
//         _endpoint->libInit( ep_cfg );

//         // Transport
//         pj::TransportConfig tcfg;
//         tcfg.port = 6061;
//         _endpoint->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

//         // Start library
//         _endpoint->libStart();
//         std::cout << "*** PJSUA2 STARTED ***" << std::endl;
//     }
//     catch(pj::Error & err)
//     {
//         std::cerr << "Exception: " << err.info() << std::endl;
//     }
// }

int main(){
    SoftphoneManager manager(6061, "192.168.1.46");

    manager.pjLibraryInit();
    manager.runSpamTest(10);

    // pj::Endpoint _endpoint;

    // pjLibraryInit(&_endpoint);

    // std::string DOMAIN = "192.168.1.46";

    // SoftphoneArguments args;
    // args.domain = DOMAIN;
    // args.secret = "12345678";
    // args.timeout = 5000;

    // args.id = "1000";
    // Softphone sp1(args);

    // args.id = "1001";
    // Softphone sp2(args);

    // args.id = "1002";
    // Softphone sp3(args);

    // std::string uri_sp1 = "sip:1000@" + args.domain;
    // std::string uri_sp2 = "sip:1001@" + args.domain;

    // sp1.call(sp2);

    // pj_thread_sleep(10000);

    // sp1.hangup();

    // pj_thread_sleep(10000);

    // sp3.call(sp1);

    // pj_thread_sleep(10000);

    // _endpoint.libDestroy();
}