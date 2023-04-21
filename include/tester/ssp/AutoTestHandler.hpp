#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>

#include "Softphone.hpp"
#include "SoftphoneArguments.hpp"
#include "../db/StreamLogger.hpp"

class AutoTestHandler
{
public:
    AutoTestHandler()
    {
    }

    ~AutoTestHandler() = default;

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
            _softphones.at(i + START_ID)->call(*_softphones.at(i + START_ID + 1));
        }
        pj_thread_sleep(TEST_SLEEP_DURATION * MILLISECONDS_TO_SECONDS);
        for(int i = 0; i < (amount * 2); i += 2)
        {
            _softphones.at(i + START_ID)->hangup();
            _cv.wait(lock, [this, i](){ return !_softphones.at(i + START_ID)->isActive(); });
        }
        _softphones.clear();
        LOG_INFO << "finished run spam test" << std::endl;
        return Logger::getInstance().closeDocument();
    }

private:
    void registerSoftphones(int amount, const std::string &domain)
    {
        _softphones.reserve(amount);

        for(int i = 0; i < amount; i++)
        {
            int softphoneID = i + START_ID;
            _softphones.emplace(softphoneID, std::make_shared<Softphone>(
                SoftphoneArguments(DEFAULT_SECRET, domain, DEFAULT_TIMEOUT, softphoneID),
                std::bind(&AutoTestHandler::onCallState, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
                std::bind(&AutoTestHandler::onRegState, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
                std::bind(&AutoTestHandler::onIncomingCall, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3)));
        }
    }

    void onCallState(const pj::OnCallStateParam &prm, const pj::CallInfo &ci, const int softphoneID)
    {
        PJ_UNUSED_ARG(prm);
        LOG_INFO << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
        {
            _softphones.at(softphoneID)->clearCall();
            onCallDisconnected();
        }
    }

    void onRegState(const pj::OnRegStateParam &prm, const pj::AccountInfo &ai, const int softphoneID) 
    {   
        LOG_INFO << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
    }

    void onIncomingCall(std::shared_ptr<SSPCall> &mainCall, std::shared_ptr<SSPCall> incomingCall, const int softphoneID)
    {
        if(mainCall && mainCall->isActive())
        {
            pj::CallOpParam opcode;
            opcode.statusCode = PJSIP_SC_BUSY_HERE;
            incomingCall->hangup(opcode);
            return;
        }
        mainCall = std::move(incomingCall);
            pj::CallInfo ci = mainCall->getInfo();
            pj::CallOpParam prm;
            LOG_INFO << "*** Incoming Call: " <<  ci.remoteUri << " ["
                    << ci.stateText << "]" << std::endl;
            std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                    << ci.stateText << "]" << std::endl;
        prm.statusCode = PJSIP_SC_OK;
        mainCall->answer(prm);
    }

    static constexpr int MILLISECONDS_TO_SECONDS = 1000;
    static constexpr int TEST_SLEEP_DURATION = 10;
    static constexpr int START_ID = 1000;

    static constexpr auto DEFAULT_SECRET = "12345678";
    static constexpr auto DEFAULT_TIMEOUT = 5000;

    std::unordered_map<int, std::shared_ptr<Softphone>> _softphones;
    std::mutex _mutex;
    std::condition_variable _cv;
};
