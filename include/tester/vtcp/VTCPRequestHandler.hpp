#pragma once

#include<iostream>
#include<functional>
#include<memory>
#include<unordered_map>

#include"VTCPOpcodes.hpp"
#include"generic/Message.hpp"

class VTCPRequestHandler
{
public:
    VTCPRequestHandler()
    {
        _handlers.insert({(int)VTCPOpcodes::onVtcpConnectRequest,std::bind(&VTCPRequestHandler::onVtcpConnectRequest, this, std::placeholders::_1)});
        _handlers.insert({(int)VTCPOpcodes::onVtcpDisconnectRequest,std::bind(&VTCPRequestHandler::onVtcpDisconnectRequest, this, std::placeholders::_1)});
        _handlers.insert({(int)VTCPOpcodes::onVtcpSspRequest,std::bind(&VTCPRequestHandler::onVtcpSspRequest, this, std::placeholders::_1)});
        _handlers.insert({(int)VTCPOpcodes::onVtcpTestRequest,std::bind(&VTCPRequestHandler::onVtcpTestRequest, this, std::placeholders::_1)});
    }

    void handle(Message message)
    {
        try
        {
            _handlers.at(message.opcode)(message.data);
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
    std::unordered_map<int, std::function<void(const std::string &)>> _handlers;
};