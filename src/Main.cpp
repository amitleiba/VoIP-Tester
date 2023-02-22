#include"tester/vtcp/VTCPServer.hpp"

int main()
{
    VTCPServer server(5060);
    server.start();
}