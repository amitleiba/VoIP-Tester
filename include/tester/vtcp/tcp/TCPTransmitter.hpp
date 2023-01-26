#pragma once

#include<iostream>
#include<functional>
#include<memory>
#include<atomic>
#include<string>

#include<boost/asio.hpp>
#include<boost/bind/bind.hpp>

using boost::asio::ip::tcp;

class TCPTransmitter
{
public:
    TCPTransmitter(std::shared_ptr<tcp::socket> socket,
    std::shared_ptr<std::atomic<bool>> active,
    std::function<void()> onDisconnect):
        _socket(std::move(socket)),
        _active(std::move(active)),
        _onDisconnect(std::move(onDisconnect))
    {

    }
    
    ~TCPTransmitter()= default;

    void write(const std::vector<std::uint8_t> message) {
        if (!*_active)
        {
            return;
        }

        boost::asio::async_write(*_socket, boost::asio::buffer(message, message.size()),
            boost::bind(&TCPTransmitter::onWrite, this, boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

private:
    void onWrite(boost::system::error_code ec, std::size_t bytesTransferred)
    {
        if(ec)
        {
            onError(ec);
            return;
        }
        std::cout<<"Message sent";
    }

    void onError(const boost::system::error_code &error)
    {
        std::cout << "*** The following exception has been thrown " << error.message() << " ***" << std::endl;
        _onDisconnect();
    }

    std::shared_ptr<tcp::socket> _socket;
    std::shared_ptr<std::atomic<bool>> _active;

    std::function<void()> _onDisconnect;
};