#pragma once

#include <iostream>
#include <functional>
#include <memory>

#include "bsoncxx/builder/stream/document.hpp"

#include "../db/Database.hpp"
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

    void init(int server_port, int sm_port, const std::string& sm_domain, int sm_log_level, const std::string& db_domain)
    {
        _manager = std::make_shared<SoftphoneManager>(sm_port, sm_domain);
        _server = std::make_shared<VTCPServer>(server_port,
            std::bind(&Application::startAutoTest, this, std::placeholders::_1),
            std::bind(&Application::startManualTest, this),
            std::bind(&Application::getHistoryHeaders, this),
            std::bind(&Application::getHistoryLog, this, std::placeholders::_1));
        _database = std::make_shared<Database>(db_domain);
        _manager->pjLibraryInit(sm_log_level);
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
        _database->save(std::move(COLLECTION_NAME), log);
        return log;
    }

    void startManualTest()
    {

    }

    bsoncxx::document::value getHistoryHeaders()
    {
        return _database->getAllHeaders(std::move(COLLECTION_NAME));
    }

    bsoncxx::document::value getHistoryLog(const std::string &docId)
    {
        auto result = _database->getLog(std::move(COLLECTION_NAME), std::move(docId));
        return result.value();
    }

    std::shared_ptr<Database> _database;
    std::shared_ptr<SoftphoneManager>_manager;
    std::shared_ptr<VTCPServer> _server;
    static constexpr auto COLLECTION_NAME = "Test-Logs";
};
