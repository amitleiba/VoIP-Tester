#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>
#include <array>
#include <atomic>

#include "Softphone.hpp"
#include "SoftphoneArguments.hpp"
#include "ManualTestOpcode.hpp"
#include "../vtcp/generic/Message.hpp"
#include "../vtcp/VTCPManualStatus.hpp"

class ManualTestHandler
{
public:
    ManualTestHandler(int sessionId):
        _sessionId(sessionId),
        _manualTestSoftphones()
    {
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_REGISTER_REQ, std::bind(&ManualTestHandler::manualTestRegister, this, std::placeholders::_1, std::placeholders::_2));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_UNREGISTER_REQ, std::bind(&ManualTestHandler::manualTestUnregister, this, std::placeholders::_1, std::placeholders::_2));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_CALL_REQ, std::bind(&ManualTestHandler::manualTestCall, this, std::placeholders::_1, std::placeholders::_2));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_HANGUP_REQ, std::bind(&ManualTestHandler::manualTestHangup, this, std::placeholders::_1, std::placeholders::_2));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_ANSWER_REQ, std::bind(&ManualTestHandler::manualTestAnswer, this, std::placeholders::_1, std::placeholders::_2));
        _manualTestHandlers.emplace(ManualTestOpcode::MANUAL_TEST_DECLINE_REQ, std::bind(&ManualTestHandler::manualTestAnswer, this, std::placeholders::_1, std::placeholders::_2));
    }

    ~ManualTestHandler() = default;

    void handleManualTest(const Message& request, Message& response)
    {
        try
        {
            auto op = request.readInteger();
            auto opcode = static_cast<ManualTestOpcode>(op);
            _manualTestHandlers.at(opcode)(request, response);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

private:
    void manualTestRegister(const Message& request, Message& response)
    {
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_REGISTER_RES));

        auto index = request.readInteger();
        auto id = request.readInteger();
        auto domain = request.readString();

        if(!(index >= 0 && index <= 2) || _manualTestSoftphones[index])
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else if(id > 0 && id < 9999)
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else
        {
            _manualTestSoftphones[index] = std::make_shared<Softphone>(
                SoftphoneArguments(DEFAULT_SECRET, std::move(domain), DEFAULT_TIMEOUT, id),
                std::bind(&ManualTestHandler::onCallState, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
                std::bind(&ManualTestHandler::onRegState, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
                std::bind(&ManualTestHandler::onIncomingCall, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::OK));
        }
    }

    void manualTestUnregister(const Message& request, Message& response)
    {
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_REGISTER_RES));

        auto index = request.readInteger();

        if(!(index >= 0 && index <= 2) || !_manualTestSoftphones[index])
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else
        {
            _manualTestSoftphones[index].reset();
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::OK));
        }
    }

    void manualTestCall(const Message& request, Message& response)
    {
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_CALL_RES));

        auto index = request.readInteger();
        auto destUri = request.readString();

        if(!(index >= 0 && index <= 2) || !_manualTestSoftphones[index])
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else
        {
            _manualTestSoftphones[index]->call(destUri);
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::OK));
        }       
    }

    void manualTestHangup(const Message& request, Message& response)
    {
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_HANGUP_RES));

        auto index = request.readInteger();

        if(!(index >= 0 && index <= 2) || !_manualTestSoftphones[index])
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else
        {
            _manualTestSoftphones[index]->hangup();
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::OK));
        }
    }
    
    void manualTestAnswer(const Message& request, Message& response)
    {
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_ANSWER_RES));

        auto index = request.readInteger();

        if(!(index >= 0 && index <= 2) || !_manualTestSoftphones[index])
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else
        {
             _manualTestSoftphones[index]->setAnsweredIncomingCall(true);
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::OK));
        }
    }

    void manualTestDecline(const Message& request, Message& response)
    {
        response.push(static_cast<int>(ManualTestOpcode::MANUAL_TEST_DECLINE_REQ));

        auto index = request.readInteger();

        if(!(index >= 0 && index <= 2) || !_manualTestSoftphones[index])
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::CLIENT_ERROR));
        else
        {
            _manualTestSoftphones[index]->setDeclinedIncomingCall(true);
            response.push(static_cast<int>(VTCP_MANUAL_STATUS::OK));
        }
    }

    void onCallState(const pj::OnCallStateParam &prm, const pj::CallInfo &ci, const int softphoneID)
    {
        std::shared_ptr<Softphone> softphone;
        for(const auto &sp : _manualTestSoftphones)
        {
            if(sp->getId() == softphoneID)
            {
                softphone = sp;
                break;
            }
        }

        PJ_UNUSED_ARG(prm);
        std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;
        if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
        {
            softphone->clearCall();
        }
    }

    void onRegState(const pj::OnRegStateParam &prm, const pj::AccountInfo &ai, const int softphoneID) 
    {   
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
             << prm.code << std::endl;
    }

    void onIncomingCall(std::shared_ptr<SSPCall> mainCall, std::shared_ptr<SSPCall> incomingCall, const int softphoneID)
    {
        if(mainCall->isActive())
        {
            pj::CallOpParam opcode;
            opcode.statusCode = PJSIP_SC_BUSY_HERE;
            incomingCall->hangup(opcode);
        }
        else
        {
            std::shared_ptr<Softphone> softphone;
            for(const auto &sp : _manualTestSoftphones)
            {
                if(sp->getId() == softphoneID)
                {
                    softphone = sp;
                    break;
                }
            }
            pj_time_val timeout;
            pj_time_val now;
            pj_gettimeofday(&timeout);
            timeout.sec += 30;

            do
            {
                pj_gettimeofday(&now);
            } while (PJ_TIME_VAL_EQ(now, timeout) || softphone->getAnsweredIncomingCall() || softphone->getDeclinedIncomingCall());
            

            if(softphone->getAnsweredIncomingCall())
            {
                mainCall = std::move(incomingCall);
                pj::CallInfo ci = mainCall->getInfo();
                pj::CallOpParam prm;
                std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
                        << ci.stateText << "]" << std::endl;
                prm.statusCode = PJSIP_SC_OK;
                mainCall->answer(prm);
            }

            if(softphone->getDeclinedIncomingCall() || PJ_TIME_VAL_EQ(now, timeout))
            {
                pj::CallOpParam opcode;
                opcode.statusCode = PJSIP_SC_DECLINE;
                incomingCall->hangup(opcode);
            }
            softphone->setAnsweredIncomingCall(false);
            softphone->setDeclinedIncomingCall(false);
        }
    }

    std::unordered_map<ManualTestOpcode, std::function<void(const Message &, Message &)>> _manualTestHandlers;
    std::array<std::shared_ptr<Softphone>, 3> _manualTestSoftphones;

    const int _sessionId;

    static constexpr int DEFAULT_ID_MULTIPLICATION_VALUE = 10000;

    static constexpr auto DEFAULT_SECRET = "12345678";
    static constexpr auto DEFAULT_TIMEOUT = 5000;
};
