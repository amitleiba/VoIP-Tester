#pragma once

#include<iostream>
#include<memory>
#include<unordered_map>

#include<boost/asio.hpp>

#include"TCPSession.hpp"
#include"TCPListener.hpp"
#include"../generic/Parser.hpp"
#include"../generic/RequestHandler.hpp"

using boost::asio::ip::tcp;

class TCPServer
{
public:
    TCPServer(const int port) :
        _listener(port, _context,
        std::bind(&TCPServer::onClientConnected, this, std::placeholders::_1, std::placeholders::_2)),
        _requestHandler(makeRequestHandler())
    {
    }

    ~TCPServer() = default;

    void start()
    {
        try {
            _context.run();
            _listener.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            throw e;
        }
    }
    
    virtual std::shared_ptr<Parser> makeParser() = 0;

    virtual std::shared_ptr<RequestHandler> makeRequestHandler() = 0;

private:
    void onClientConnected(const std::size_t id, tcp::socket placeholder)
    {
        auto session = std::make_shared<TCPSession>(placeholder, id, makeParser(),
            std::bind(&TCPServer::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
        _sessions.emplace(id, session);
        _sessions.at(id)->start();
    }

    void onMessageReceived(const int id, std::shared_ptr<Message> message) 
    {
        _requestHandler->handle(message);
    }

    boost::asio::io_context _context;
    TCPListener _listener;

    std::unordered_map<int , std::shared_ptr<TCPSession>> _sessions;
    std::shared_ptr<RequestHandler> _requestHandler;
};