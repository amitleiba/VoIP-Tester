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
    std::function<void(const std::string &)> onDataReceived):
        _socket(std::move(socket)),
        _active(std::move(active)),
        _onDataReceived(std::move(onDataReceived))
    {
        
    }
    
    ~TCPReceiver() = default;

    void run()
    {
        if (!*_active)
        {
            *_active = true;
            read();
        }
    }

    void stop()
    {
        *_active = false;
        //_onClientDisconnected()?
    }

private:
    void read() {
        if (*_active)
        {
            _messageHeaderBuffer.resize(HEADER_LENGTH);
            boost::asio::async_read(_socket,
                boost::asio::buffer(_messageHeaderBuffer, HEADER_LENGTH),
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
            _messageDataBuffer.resize(std::atoi(_messageHeaderBuffer.data()));
            boost::asio::async_read(_socket,
                boost::asio::buffer(_messageDataBuffer, HEADER_LENGTH),
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
        //Message _readMessage = _parser.deserialize(_readBody);
        read();
    }

    void onError(const boost::system::error_code &error)
    {
        std::cout << "*** The following exception has been thrown " << error.message() << " ***" << std::endl;
        stop();
    }

    static constexpr int HEADER_LENGTH = 4;

    std::shared_ptr<tcp::socket> _socket;
    std::string _messageHeaderBuffer;
    std::string _messageDataBuffer;
    std::shared_ptr<std::atomic<bool>> _active;

    std::function<void(const std::string &)> _onDataReceived;
};