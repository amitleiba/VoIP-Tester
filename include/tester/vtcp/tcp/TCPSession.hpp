#pragma once

#include<iostream>
#include<memory>
#include<functional>

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
    std::function<void(const int, Message)> onMessageReceived) : 
        _socket(std::make_shared<tcp::socket>(std::move(socket))),
        _parser(makeParser()),
        _onMessageReceived(onMessageReceived),
        _receiver(_socket,
            std::bind(&TCPSession::onDataReceived, this, std::placeholders::_1)),
        _transmitter(_socket)
    {
        _id = id;
    }

    ~TCPSession() = default;

    void start()
    {
        _receiver.run();
    }

    void send(Message message)
    {
        std::string serializedMessage = _parser->serialize(message);
        _transmitter.write(serializedMessage);
    }

private:
    void onDataReceived(const std::string & data)
    {
        Message message = _parser->deserialize(data);
        _onMessageReceived(_id, message);
    }

    void  onClientDisconnected()
    {
        _receiver.stop();
    }

    int _id;

    std::shared_ptr<tcp::socket> _socket;

    std::shared_ptr<Parser> _parser;
    TCPReceiver _receiver;
    TCPTransmitter _transmitter;

    std::function<void(const int, Message)> _onMessageReceived;
};