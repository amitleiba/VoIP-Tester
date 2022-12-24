#pragma once

#include<iostream>

#include"../db/Database.hpp"
#include"../ssp/SoftphoneManager.hpp"
#include"../vtcp/VTCPServer.hpp"

class Application
{
private:
    Database *_db;
    SoftphoneManager *_sm;
    VTCPServer *_vs;
public:
    Application(/* args */);
    ~Application();
};
