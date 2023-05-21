#include "tester/vtcp/VTCPServer.hpp"

int main()
{
    VTCPServer server(8080, 5060, 5, "192.168.1.123");
    server.start();
}