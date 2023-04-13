#include "tester/vtcp/VTCPServer.hpp"

int main()
{
    VTCPServer server = VTCPServer(8080, 5060, 0, "192.168.1.123");
    server.start();
}