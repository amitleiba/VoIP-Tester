#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>

#include <pjsua2.hpp>

#include "bsoncxx/builder/stream/document.hpp"

#include "Softphone.hpp"
#include "SoftphoneArguments.hpp"
#include "ManualTestOpcode.hpp"
#include "../db/StreamLogger.hpp"

class SoftphoneManager
{
public:
    SoftphoneManager(int port):
        _port(port)
    {
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_REGISTER_REQ, std::bind(&SoftphoneManager::manualTestRegister, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_CALL_REQ, std::bind(&SoftphoneManager::manualTestCall, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_ANSWER_REQ, std::bind(&SoftphoneManager::manualTestAnswer, this, std::placeholders::_1));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_HANGUP_REQ, std::bind(&SoftphoneManager::manualTestHangup, this, std::placeholders::_1));
    }

    ~SoftphoneManager()
    {
        _endpoint.libDestroy();
        _softphones.clear();  //TODO
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

    void onCallDisconnected()
    {
        _cv.notify_one();
    }

    bsoncxx::document::value runSpamTest(int amount, std::string domain)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        Logger::getInstance().openDocument();
        LOG_INFO << "started run spam test" << std::endl;
        registerSoftphones(amount * 2, std::move(domain));
        pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        for(int i = 0; i < (amount * 2); i += 2)
        {
            _softphones.at(i)->call(*_softphones.at(i + 1));
        }
        pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        for(int i = 0; i < (amount * 2); i += 2)
        {
            _softphones.at(i)->hangup();
            _cv.wait(lock, [this, i](){ return !_softphones.at(i)->isActive(); });
        }
        _softphones.clear();
        LOG_INFO << "finished run spam test" << std::endl;
        return Logger::getInstance().closeDocument();
    }

    Message handleManualTest(const Message& request)
    {
        try
        {
            auto op = request.readInteger();
            auto opcode = static_cast<ManualTestOpcode>(op);
            return _manualTestHandlers.at(opcode)(request);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return Message();
        }
    }

private:
    void registerSoftphones(int amount, std::string domain)
    {
        _softphones.reserve(amount);

        for(int i = 0; i < amount; i++)
        {
            _softphones.emplace_back(std::make_shared<Softphone>(
                SoftphoneArguments("12345678", std::move(domain), 5000, std::to_string(i + START_URI)),
                std::bind(&SoftphoneManager::onCallDisconnected, this)));
        }
    }

    Message manualTestRegister(const Message& request)
    {
        //getting the required arguments for the function from the request
        Message response;
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_REGISTER_RES));
        Logger::getInstance().openDocument();
        //function in softphone that we want to activate
        response.push(bsoncxx::to_json(Logger::getInstance().closeDocument().view()));
        return response;
    }

    Message manualTestCall(const Message& request)
    {
        //getting the required arguments for the function from the request
        Message response;
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_CALL_RES));
        Logger::getInstance().openDocument();
        //function in softphone that we want to activate
        response.push(bsoncxx::to_json(Logger::getInstance().closeDocument().view()));
        return response;
    }

    Message manualTestAnswer(const Message& request)
    {
        //getting the required arguments for the function from the request
        Message response;
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_ANSWER_RES));
        Logger::getInstance().openDocument();
        //function in softphone that we want to activate
        response.push(bsoncxx::to_json(Logger::getInstance().closeDocument().view()));
        return response;
    }

    Message manualTestHangup(const Message& request)
    {
        //getting the required arguments for the function from the request
        Message response;
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_HANGUP_RES));
        Logger::getInstance().openDocument();
        //function in softphone that we want to activate
        response.push(bsoncxx::to_json(Logger::getInstance().closeDocument().view()));
        return response;
    }

    static constexpr int MILLISECONDS_TO_SECONDS = 1000;
    static constexpr int TEST_SLEEP_DURATION = 10;
    static constexpr int START_URI = 1000;

    const int _port;
    pj::Endpoint _endpoint;
    std::vector<std::shared_ptr<Softphone>> _softphones;
    std::mutex _mutex;
    std::condition_variable _cv;

    std::unordered_map<ManualTestOpcode, std::function<Message(const Message &)>> _manualTestHandlers;
};
