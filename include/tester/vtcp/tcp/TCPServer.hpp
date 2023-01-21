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
        _listener(port, _io_context,
        std::bind(&TCPServer::onClientConnected, this, std::placeholders::_1, std::placeholders::_2))
    {
        _handler = makeRequestHandler();
    }

    ~TCPServer() = default;

    void start()
    {
        try {
            _io_context.run();
            _listener.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            throw e;
        }
    }

    void onClientConnected(const std::size_t id, tcp::socket placeholder)
    {
        auto session = std::make_shared<TCPSession>(placeholder, std::bind(&TCPServer::makeParser, this));
        _sessions.emplace(id, session);
        _sessions.at(id)->start();
    }

    void onMessageReceived(const int id, std::shared_ptr<Message> message) 
    {
        _handler->handle(message);
        onCompletion(id);
    }

    void onCompletion(const int id)
    {
        std::cout << "Done" << std::endl;
        //maybe send something to the client?
        //_sessions.at(id)->send(message)
    }
    
    virtual std::shared_ptr<Parser> makeParser() = 0;

    virtual std::shared_ptr<RequestHandler> makeRequestHandler() = 0;

protected:
    boost::asio::io_context _io_context;
    std::unordered_map<int , std::shared_ptr<TCPSession>> _sessions;
    TCPListener _listener;
    std::shared_ptr<RequestHandler> _handler;

private:
    
};