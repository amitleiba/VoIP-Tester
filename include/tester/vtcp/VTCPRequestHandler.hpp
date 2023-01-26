#pragma once

#include<functional>
#include<memory>
#include<unordered_map>

#include"generic/RequestHandler.hpp"
#include"VTCPOpcode.hpp"

class VTCPRequestHandler : public RequestHandler
{
public:
    VTCPRequestHandler():
        RequestHandler()
    {
        _handlers.emplace(VTCPOpcode::VTCP_CONNECT_REQ,std::bind(&VTCPRequestHandler::onVtcpConnect, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_DISCONNECT_REQ,std::bind(&VTCPRequestHandler::onVtcpDisconnect, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_MANUAL_TEST_REQ,std::bind(&VTCPRequestHandler::onVtcpManualTest, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_AUTO_TEST_REQ,std::bind(&VTCPRequestHandler::onVtcpAutoTest, this, std::placeholders::_1, std::placeholders::_2));
    }

    Message handle(std::shared_ptr<TCPSession> sender, const Message& request) override
    {
        try
        {
            auto opcode = static_cast<VTCPOpcode>(request.readInteger());
            _handlers.at(opcode)(std::move(sender), request);
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    ~VTCPRequestHandler() = default;

    void onVtcpConnect(std::shared_ptr<TCPSession> sender, const Message & data)
    {
        std::cout << "New client connected" << std::endl;
    }

    void onVtcpDisconnect(std::shared_ptr<TCPSession> sender, const Message & data)
    {
        std::cout << "Client disconnected" << std::endl;

        //sender->onDisconnect();

    }

    void onVtcpAutoTest(std::shared_ptr<TCPSession> sender, const Message & request)
    {
        std::cout << "Client requested Auto test" << std::endl;

        std::string domain = request.readString();
        int amount = request.readInteger();

        std::cout << "Domian: " << domain << " Amount: " << amount << std::endl;

        // SoftphoneManager manager(5060, domain);

        // manager.pjLibraryInit(0);
        // manager.runSpamTest(amount);
    }

    void onVtcpManualTest(std::shared_ptr<TCPSession> sender, const Message & data)
    {
        std::cout << "Client requested Manual test" << std::endl;
    }


private:
    std::unordered_map<VTCPOpcode, std::function<void(std::shared_ptr<TCPSession>, const Message &)>> _handlers;
};