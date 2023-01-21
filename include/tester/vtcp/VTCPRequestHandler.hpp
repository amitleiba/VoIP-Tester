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
        _handlers.emplace(VTCPOpcode::ON_VTCP_CONNECT_REQUEST,std::bind(&VTCPRequestHandler::onVtcpConnectRequest, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::ON_VTCP_DISCONNECT_REQUEST,std::bind(&VTCPRequestHandler::onVtcpDisconnectRequest, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::ON_VTCP_SSP_REQUEST,std::bind(&VTCPRequestHandler::onVtcpSspRequest, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::ON_VTCP_TEST_REQUEST,std::bind(&VTCPRequestHandler::onVtcpTestRequest, this, std::placeholders::_1));
    }

    void handle(std::shared_ptr<Message> message) override
    {
        try
        {
            auto vtcpMessage = std::dynamic_pointer_cast<VTCPMessage>(message);
            _handlers.at(vtcpMessage->getOpcode())(vtcpMessage->getData());
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    ~VTCPRequestHandler() = default;

    void onVtcpConnectRequest(const std::string & data)
    {

    }

    void onVtcpDisconnectRequest(const std::string & data)
    {

    }

    void onVtcpTestRequest(const std::string & data)
    {
        
    }

    void onVtcpSspRequest(const std::string & data)
    {
        
    }


private:
    std::unordered_map<VTCPOpcode, std::function<void(const std::string &)>> _handlers;
};