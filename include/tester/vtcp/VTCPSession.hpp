#pragma once

#include<functional>
#include<memory>
#include<unordered_map>

#include"tcp/TCPSession.hpp"
#include"VTCPOpcode.hpp"

class VTCPSession : public TCPSession
{
public:
    VTCPSession(tcp::socket socket, const std::size_t id,
    std::function<void(const std::size_t, const Message&)> onMessageReceived,
    std::function<void(const std::size_t)> onDisconnect):
        TCPSession(std::move(socket), id, onMessageReceived, onDisconnect)
    {
        _handlers.emplace(VTCPOpcode::VTCP_CONNECT_REQ, std::bind(&VTCPSession::onVtcpConnect, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_DISCONNECT_REQ, std::bind(&VTCPSession::onVtcpDisconnect, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_AUTO_TEST_REQ, std::bind(&VTCPSession::onVtcpAutoTest, this, std::placeholders::_1));
        _handlers.emplace(VTCPOpcode::VTCP_MANUAL_TEST_REQ, std::bind(&VTCPSession::onVtcpManualTest, this, std::placeholders::_1));
    }

    void handle(const Message& request) override
    {
        try
        {
            auto opcode = static_cast<VTCPOpcode>(request.readInteger());
            _handlers.at(opcode)(request);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    ~VTCPSession() = default;

    void onVtcpConnect(const Message & request)
    {
        std::cout << "New client connected" << std::endl;
        Message response;
        response.push(static_cast<int>(VTCPOpcode::VTCP_CONNECT_RES));
        std::string result_msg = "Hello new client";
        response.push(result_msg);
        response.pushSize();
        send(response);
    }

    void onVtcpDisconnect(const Message & request)
    {
        std::cout << "Client disconnected" << std::endl;

        disconnect();
    }

    void onVtcpAutoTest(const Message & request)
    {
        std::cout << "Client requested Auto test" << std::endl;

        std::string domain = request.readString();
        int amount = request.readInteger();

        std::cout << "Domain: " << domain << " Amount: " << amount << std::endl;

        // SoftphoneManager manager(5060, domain);

        // manager.pjLibraryInit(0);
        // manager.runSpamTest(amount);
    }

    void onVtcpManualTest(const Message & request)
    {
        std::cout << "Client requested Manual test" << std::endl;
    }

private:
    std::unordered_map<VTCPOpcode, std::function<void(const Message &)>> _handlers;
};