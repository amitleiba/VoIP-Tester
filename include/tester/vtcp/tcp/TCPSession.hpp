#pragma once

#include<memory>
#include<functional>
#include<atomic>
#include<string>

#include<boost/asio.hpp>

#include"TCPReceiver.hpp"
#include"TCPTransmitter.hpp"
#include"../generic/Message.hpp"

using  boost::asio::ip::tcp;

class TCPSession
{
public:
    TCPSession(tcp::socket socket, const std::size_t id,
    std::function<void(const std::size_t, const Message&)> onMessageReceived,
    std::function<void(const std::size_t)> onDisconnect) : 
        _id(id),
        _socket(std::make_shared<tcp::socket>(std::move(socket))),
        _active(std::make_shared<std::atomic<bool>>(false)),
        _onMessageReceived(onMessageReceived),
        _onDisconnect(onDisconnect),
        _receiver(_socket, _active,
            std::bind(&TCPSession::onDataReceived, this, std::placeholders::_1),
            std::bind(&TCPSession::onDisconnect, this)),
        _transmitter(_socket, _active,
            std::bind(&TCPSession::onDisconnect, this))
    { 
    }

    virtual ~TCPSession() = default;

    void start()
    {
        if(!*_active)
        {
            *_active = true;
            _receiver.run();
        }
    }

    void send(const Message & message)
    {
        _transmitter.write(message.getPayload());
    }

    void disconnect()
    {
        onDisconnect();
    }

    virtual void handle(const Message& request) = 0;

private:
    void onDisconnect()
    {
        *_active = false;
        _onDisconnect(_id);
    }

    void onDataReceived(std::vector<std::uint8_t> data)
    {
        _onMessageReceived(_id, Message(data));
    }
    
    std::size_t _id;

    std::shared_ptr<tcp::socket> _socket;
    std::shared_ptr<std::atomic<bool>> _active;

    TCPReceiver _receiver;
    TCPTransmitter _transmitter;

    std::function<void(const int, const Message &)> _onMessageReceived;
    std::function<void(const int)> _onDisconnect;
};