#pragma once

#include <iostream>
#include <functional>
#include <memory>

#include "bsoncxx/builder/stream/document.hpp"

#include "../db/VTDatabase.hpp"
#include "../vtcp/VTCPServer.hpp"
#include "../ssp/SoftphoneManager.hpp"
#include "../db/Logger.hpp"

class Application
{
public:
    static Application& getInstance()
    {
        static Application instance;
        return instance;
    }

    ~Application() = default;

    void init(int serverPort, int softphoneManagerPort, const std::string& softphoneManagerDomain,
        int softphoneManagerLogLevel, const std::string& db_domain)
    {
        _manager = std::make_shared<SoftphoneManager>(softphoneManagerPort, softphoneManagerDomain);
        _server = std::make_shared<VTCPServer>(serverPort,
            std::bind(&Application::startAutoTest, this, std::placeholders::_1),
            std::bind(&Application::startManualTest, this),
            std::bind(&Application::getHistoryHeaders, this),
            std::bind(&Application::getHistoryLog, this, std::placeholders::_1));
        _database = std::make_shared<VTDatabase>(db_domain);
        _manager->pjLibraryInit(softphoneManagerLogLevel);
    }

    void run()
    {
        _server->start();
    }

private:
    Application()
    {

    }

    bsoncxx::document::value startAutoTest(int amount)
    {
        auto log = _manager->runSpamTest(amount);
        _database->save(log);
        return log;
    }

    void startManualTest()
    {

    }

    bsoncxx::document::value getHistoryHeaders()
    {
        return _database->getAllHeaders();
    }

    bsoncxx::document::value getHistoryLog(const std::string &docId)
    {
        auto result = _database->getDocument(std::move(docId));
        return result.value();
    }

    std::shared_ptr<VTDatabase> _database;
    std::shared_ptr<SoftphoneManager>_manager;
    std::shared_ptr<VTCPServer> _server;
};
