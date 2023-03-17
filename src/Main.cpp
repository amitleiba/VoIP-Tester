#include "../include/tester/core/Application.hpp"

int main()
{
    auto app = Application::getInstance();
    app.init(8080, 5060, "192.168.1.123", 0, "192.168.1.123");
    app.run();
}