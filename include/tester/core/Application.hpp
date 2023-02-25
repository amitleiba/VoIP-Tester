#pragma once

#include <iostream>
#include <functional>
#include <memory>

#include "../db/Database.hpp"
#include "../vtcp/VTCPServer.hpp"
#include "../ssp/SoftphoneManager.hpp"

class Application
{
public:
    Application& getInstance()
    {
        static Application instance;
        return instance;
    }

    Application()
    {

    }

    ~Application() = default;

    void init(int server_port, int sm_port, const std::string& sm_domain, int sm_log_level, const std::string& db_domain)
    {
        _sm = std::make_shared<SoftphoneManager>(sm_port, sm_domain,
            std::bind(&Application::onComplete, this));
        _vs = std::make_shared<VTCPServer>(server_port,
            std::bind(&Application::startAutoTest, this, std::placeholders::_1),
            std::bind(&Application::startManualTest, this));
        _db = std::make_shared<Database>(db_domain);
        _sm->pjLibraryInit(sm_log_level);
    }

    void run()
    {
        _vs->start();
    }

private:

    void startAutoTest(int amount)
    {

    }

    void startManualTest()
    {

    }

    void onComplete()
    {

    }

    std::shared_ptr<Database> _db;
    std::shared_ptr<SoftphoneManager>_sm;
    std::shared_ptr<VTCPServer> _vs;
};
