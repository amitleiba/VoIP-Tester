// #include "../include/tester/core/Application.hpp"
#include "../include/tester/db/StreamLogger.hpp"

int main()
{
    // auto app = Application::getInstance();
    // app.init(8080, 5060, "192.168.14.93", 0, "192.168.14.93");
    // app.run();
    Logger::getInstance();
    Logger::getInstance().openDocument();
    LOG_INFO << "Hello world" << std::endl;
    std::cout << bsoncxx::to_json(Logger::getInstance().closeDocument().view()) << std::endl;
}