#include "tester/vtcp/VTCPServer.hpp"

int main()
{
    std::shared_ptr<VTCPServer> server = std::make_shared<VTCPServer>(8080);
    server->init(5060, 0, "192.168.1.123");
    server->start();
}