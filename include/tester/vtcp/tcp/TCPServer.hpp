#pragma once

#include<iostream>
#include<memory>
#include<unordered_map>

#include<boost/asio.hpp>

#include"TCPSession.hpp"
#include"TCPListener.hpp"

class TCPServer
{
public:
    TCPServer(const int port) :
        _listener(port, _io_context,
        std::bind(&TCPServer::onClientConnected, this, std::placeholders::_1, std::placeholders::_2))
    {

    }

    ~TCPServer() = default;

    void start()
    {
        try {
            _io_context.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            throw e;
        }
    }

    void onClientConnected(const int id, const boost::asio::ip::tcp::socket &placeholder)
    {
        auto session = std::make_shared<TCPSession>(placeholder);
        _sessions.insert({id, session});
        _sessions.at(id)->start();
        _listener.accept();
    }

private:
    boost::asio::io_context _io_context;
    std::unordered_map<int , std::shared_ptr<TCPSession>> _sessions;
    TCPListener _listener;
};