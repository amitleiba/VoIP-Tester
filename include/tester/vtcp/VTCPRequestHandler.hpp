#pragma once

#include<functional>
#include<memory>
#include<unordered_map>

#include"generic/RequestHandler.hpp"
#include"VTCPOpcode.hpp"
#include"VTCPMessage.hpp"

#include"tester/ssp/SoftphoneManager.hpp"

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
        std::cout << "New client connected" << std::endl;
    }

    void onVtcpDisconnect(const std::string & data)
    {
        std::cout << "Client disconnected" << std::endl;
    }

    void onVtcpAutoTest(const std::string & data)
    {
        std::cout << "Client requested Auto test" << std::endl;
        
        std::vector<std::string> vector_data;
        std::stringstream ss(data);
        std::string item;

        while (std::getline(ss, item, ',')) {
            vector_data.push_back(item);
        }

        std::string domain = vector_data.at(0);
        int amount = std::stoi(vector_data.at(1));

        std::cout << "Domian: " << domain << " Amount: " << amount << std::endl; 

        // SoftphoneManager manager(5060, "192.168.132.93");

        // manager.pjLibraryInit(0);
        // manager.runSpamTest(100);
    }

    void onVtcpManualTest(const std::string & data)
    {
        std::cout << "Client requested Manual test" << std::endl;
        std::cout << "The amout of softphones that the client wanted" << std::stoi(data) << std::endl;
    }


private:
    std::unordered_map<VTCPOpcode, std::function<void(const std::string &)>> _handlers;
};