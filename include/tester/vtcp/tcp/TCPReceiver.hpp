#pragma once

#include<iostream>
#include<functional>
#include<memory>
#include<atomic>
#include<string>

#include<boost/asio.hpp>
#include<boost/bind/bind.hpp>

using boost::asio::ip::tcp;

class TCPReceiver
{
public:
    TCPReceiver(std::shared_ptr<tcp::socket> socket,
    std::shared_ptr<std::atomic<bool>> active,
    std::function<void(const std::string &)> onDataReceived,
    std::function<void()> onDisconnect):
        _socket(std::move(socket)),
        _active(std::move(active)),
        _onDataReceived(std::move(onDataReceived)),
        _onDisconnect(std::move(onDisconnect))
    {
        
    }
    
    ~TCPReceiver() = default;

    void run()
    {
        read();
    }

private:
    void read() {
        if (*_active)
        {
            _messageHeaderBuffer.resize(Message::HEADER_LENGTH);
            boost::asio::async_read(_socket,
                boost::asio::buffer(_messageHeaderBuffer, Message::HEADER_LENGTH),
                boost::bind(&TCPReceiver::onReadHeader, this, boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }    
    }

    void onReadHeader(boost::system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            onError(ec);
            return;
        }
        if(*_active)
        {
            std::size_t data_size = std::atoi(_messageHeaderBuffer.data());
            _messageDataBuffer.resize(data_size);
            boost::asio::async_read(_socket,
                boost::asio::buffer(_messageDataBuffer, data_size),
                boost::bind(&TCPReceiver::onReadData, this, boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }

    void onReadData(const boost::system::error_code & ec, size_t bytesTransferred)
    {
        if (ec) 
        {
            onError(ec);
            return; 
		}
        _onDataReceived(_messageDataBuffer);
        read();
    }

    void onError(const boost::system::error_code &error)
    {
        std::cout << "*** The following exception has been thrown " << error.message() << " ***" << std::endl;
        _onDisconnect();
    }
    
    std::shared_ptr<tcp::socket> _socket;
    std::string _messageHeaderBuffer;
    std::string _messageDataBuffer;
    std::shared_ptr<std::atomic<bool>> _active;

    std::function<void(const std::string &)> _onDataReceived;
    std::function<void()> _onDisconnect;
};