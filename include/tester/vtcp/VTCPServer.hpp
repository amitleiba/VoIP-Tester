#pragma once

#include <functional>
#include <vector>

#include "tcp/TCPServer.hpp"
#include "VTCPOpcode.hpp"

#include "../db/VTDatabase.hpp"
#include "../ssp/SoftphoneManager.hpp"

class VTCPServer : public TCPServer
{
public:
    VTCPServer(const std::uint16_t port) :
        TCPServer(port)
    {
        _handlers.emplace(VTCPOpcode::VTCP_CONNECT_REQ, std::bind(&VTCPServer::onVtcpConnect, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_DISCONNECT_REQ, std::bind(&VTCPServer::onVtcpDisconnect, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_AUTO_TEST_REQ, std::bind(&VTCPServer::onVtcpAutoTest, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_MANUAL_TEST_REQ, std::bind(&VTCPServer::onVtcpManualTest, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_HISTORY_HEADER_REQ, std::bind(&VTCPServer::onVtcpHistoryHeader, this, std::placeholders::_1, std::placeholders::_2));
        _handlers.emplace(VTCPOpcode::VTCP_HISTORY_LOG_REQ, std::bind(&VTCPServer::onVtcpHistoryLog, this, std::placeholders::_1, std::placeholders::_2));
    }

    ~VTCPServer() = default;

    void init(int softphoneManagerPort, int softphoneManagerLogLevel, std::string databaseDomain)
    {
        _manager = std::make_shared<SoftphoneManager>(softphoneManagerPort);
        _database = std::make_shared<VTDatabase>(std::move(databaseDomain));
        _manager->pjLibraryInit(softphoneManagerLogLevel);
    }

private:
    void handle(const std::size_t id, const Message& request) override
    {
        try
        {
            auto op = request.readInteger();
            std::cout << op << std::endl;
            auto opcode = static_cast<VTCPOpcode>(op);
            _handlers.at(opcode)(id, request);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void onVtcpConnect(const std::size_t id, const Message & request)
    {
        std::cout << "New client connected" << std::endl;

        Message response;
        response.push(static_cast<int>(VTCPOpcode::VTCP_CONNECT_RES));
        send(id, response);
    }

    void onVtcpDisconnect(const std::size_t id, const Message & request)
    {
        std::cout << "Client disconnected" << std::endl;

        try
        {
            _sessions.at(id)->disconnect();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void onVtcpAutoTest(const std::size_t id, const Message & request)
    {
        std::cout << "Client requested Auto test" << std::endl;

        std::string domain = request.readString();
        int amount = request.readInteger();

        std::cout << "Domain: " << domain << " Amount: " << amount << std::endl;

        auto log = _manager->runSpamTest(amount, domain);
        _database->saveTestLog(log);

        Message response;

        response.push(static_cast<int>(VTCPOpcode::VTCP_AUTO_TEST_RES));
        response.push(bsoncxx::to_json(log.view()));
        send(id, response);
    }

    void onVtcpManualTest(const std::size_t id, const Message & request)
    {
        std::cout << "Client requested Manual test" << std::endl;

        // _startManualTest();
    }

    void onVtcpHistoryHeader(const std::size_t id, const Message & request)
    {
        Message response;

        response.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_HEADER_RES));
        response.push(bsoncxx::to_json(_database->getTestLogsHistory()));
        send(id, response);
    }

    void onVtcpHistoryLog(const std::size_t id, const Message & request)
    {
        auto documentId = request.readString();
        Message response;

        response.push(static_cast<int>(VTCPOpcode::VTCP_HISTORY_LOG_RES));
        response.push(bsoncxx::to_json(_database->getHistoryLog(std::move(documentId)).value().view()));
        send(id, response);
    }

    void onSessionOpened(const std::size_t id)
    {

    }
    
    void onSessionClosed(const std::size_t id)
    {

    }

    std::shared_ptr<VTDatabase> _database;
    std::shared_ptr<SoftphoneManager>_manager;

    std::unordered_map<VTCPOpcode, std::function<void(int ,const Message &)>> _handlers;
    std::unordered_map<std::size_t, std::vector<Softphone>> manualTestSoftphones;

    static constexpr int START_ID = 9997;
    static constexpr int END_ID = 9999;
};