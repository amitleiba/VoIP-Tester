#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <type_traits>

#include <boost/asio.hpp>

#include "TCPSession.hpp"
#include "TCPListener.hpp"
#include "../generic/Message.hpp"

using boost::asio::ip::tcp;

class TCPServer
{
public:
    TCPServer(const std::uint16_t port) :
        _listener(port, _context,
        std::bind(&TCPServer::onClientConnected, this, std::placeholders::_1, std::placeholders::_2))
    {
    }

    ~TCPServer() = default;

    void start()
    {
        try {
            _listener.run();
            _context.run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            throw e;
        }
    }

protected:    
    boost::asio::io_context _context;

    std::unordered_map<std::size_t, std::shared_ptr<TCPSession>> _sessions;

    virtual void handle(const std::size_t id, const Message& request) = 0;
    virtual void onSessionOpened(const std::size_t id) = 0;
    virtual void onSessionClosed(const std::size_t id) = 0;


    void send(const std::size_t id, const Message& message) 
    {
        try
        {
            _sessions.at(id)->send(message);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void broadcast(const Message& message)
    {
        for(const auto& [_, session] :_sessions)
        {
            session->send(message);
        }
    }

private:
    void onClientConnected(const std::size_t id, tcp::socket socket)
    {
        std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
            << " : " << socket.remote_endpoint().port() << std::endl;
        auto session = std::make_shared<TCPSession>(id, std::move(socket),
            std::bind(&TCPServer::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&TCPServer::onDisconnect, this, std::placeholders::_1));
        _sessions.emplace(id, std::move(session));
        _sessions.at(id)->start();
        onSessionOpened(id);
    }

    void onMessageReceived(const std::size_t id, const Message& request) 
    {
        handle(id, request);
    }

    void onDisconnect(const std::size_t id)
    {
        _sessions.erase(id);
        onSessionClosed(id);
    }

    TCPListener _listener;
};