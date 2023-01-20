#pragma once

#include<iostream>
#include<memory>
#include<functional>
#include<atomic>

#include<boost/asio.hpp>

#include"TCPReceiver.hpp"
#include"TCPTransmitter.hpp"
#include"../generic/Parser.hpp"
#include"../generic/Message.hpp"

using  boost::asio::ip::tcp;

class TCPSession
{
public:
    TCPSession(tcp::socket socket, const int id,
    std::function<std::shared_ptr<Parser>()> makeParser,
    std::function<void(const int, std::shared_ptr<Message>)> onMessageReceived) : 
        _socket(std::make_shared<tcp::socket>(std::move(socket))),
        _parser(makeParser()),
        _active(std::make_shared<std::atomic<bool>>(false)),
        _onMessageReceived(onMessageReceived),
        _receiver(_socket, _active,
            std::bind(&TCPSession::onDataReceived, this, std::placeholders::_1),
            std::bind(&TCPSession::onDisconnect, this)),
        _transmitter(_socket, _active,
            std::bind(&TCPSession::onDisconnect, this))
    {
        _id = id;
    }

    ~TCPSession() = default;

    void start()
    {
        if(!*_active)
        {
            *_active = true;
            _receiver.run();
        }
    }

    void send(Message message)
    {
        std::string serializedMessage = _parser->serialize(std::make_shared<Message>(message));
        _transmitter.write(serializedMessage);
    }

private:
    void onDataReceived(const std::string & data)
    {
        auto message = _parser->deserialize(data);
        _onMessageReceived(_id, message);
    }

    void  onDisconnect()
    {
        *_active = false;
    }

    std::size_t _id;

    std::shared_ptr<tcp::socket> _socket;
    std::shared_ptr<std::atomic<bool>> _active;

    std::shared_ptr<Parser> _parser;
    TCPReceiver _receiver;
    TCPTransmitter _transmitter;

    std::function<void(const int, std::shared_ptr<Message>)> _onMessageReceived;
};