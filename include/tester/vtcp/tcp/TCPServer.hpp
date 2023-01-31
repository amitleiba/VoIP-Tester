#pragma once

#include<iostream>
#include<memory>
#include<unordered_map>

#include<boost/asio.hpp>

#include"TCPSession.hpp"
#include"TCPListener.hpp"
#include"../generic/RequestHandler.hpp"

using boost::asio::ip::tcp;

class TCPServer
{
public:
    TCPServer(const std::uint16_t port, std::shared_ptr<RequestHandler> requestHandler) :
        _listener(port, _context,
        std::bind(&TCPServer::onClientConnected, this, std::placeholders::_1, std::placeholders::_2)),
        _requestHandler(std::move(requestHandler))
    {    }

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

    std::unordered_map<std::size_t , std::shared_ptr<TCPSession>> _sessions;

private:
    void onClientConnected(const std::size_t id, tcp::socket socket)
    {
        std::cout << "Accepted connection from " << socket.remote_endpoint().address().to_string()
            << " : " << socket.remote_endpoint().port() << std::endl;
        auto session = std::make_shared<TCPSession>(std::move(socket), id,
            std::bind(&TCPServer::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&TCPServer::onDisconnect, this, std::placeholders::_1));
        _sessions.emplace(id, std::move(session));
        _sessions.at(id)->start();
    }

    void onMessageReceived(const std::size_t id, const Message& request) 
    {
        _requestHandler->handle(_sessions.at(id), request);
    }

    void onDisconnect(const std::size_t id)
    {
        _sessions.erase(id);
    }

    TCPListener _listener;

    std::shared_ptr<RequestHandler> _requestHandler;
};