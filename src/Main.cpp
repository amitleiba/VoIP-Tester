#include "tester/vtcp/VTCPServer.hpp"

int main()
{
    VTCPServer server(8080, 5060, 0, "192.168.12.93");
    server.start();
}