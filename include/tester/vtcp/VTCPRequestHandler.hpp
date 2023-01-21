#pragma once

#include<functional>
#include<memory>
#include<unordered_map>

#include"generic/RequestHandler.hpp"
#include"VTCPOpcode.hpp"
#include"VTCPMessage.hpp"

class VTCPRequestHandler : public RequestHandler
{
public:
    VTCPRequestHandler():
        RequestHandler()
    {
        _handlers.emplace(VTCPOpcode::VTCP_CONNECT,std::bind(&VTCPRequestHandler::onVtcpConnect, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_DISCONNECT,std::bind(&VTCPRequestHandler::onVtcpDisconnect, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_MANUAL_TEST,std::bind(&VTCPRequestHandler::onVtcpManualTest, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_AUTO_TEST,std::bind(&VTCPRequestHandler::onVtcpAutoTest, this, std::placeholders::_1));
    }

    void handle(std::shared_ptr<Message> message) override
    {
        try
        {
            auto vtcpMessage = std::static_pointer_cast<VTCPMessage>(message);
            _handlers.at(vtcpMessage->getOpcode())(vtcpMessage->getData());
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    ~VTCPRequestHandler() = default;

    void onVtcpConnect(const std::string & data)
    {

    }

    void onVtcpDisconnect(const std::string & data)
    {

    }

    void onVtcpAutoTest(const std::string & data)
    {
        
    }

    void onVtcpManualTest(const std::string & data)
    {
        
    }


private:
    std::unordered_map<VTCPOpcode, std::function<void(const std::string &)>> _handlers;
};