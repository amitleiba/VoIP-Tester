#pragma once

#include<iostream>
#include<functional>

#include<boost/asio.hpp>
#include<boost/bind/bind.hpp>

using boost::asio::ip::tcp;

class TCPListener
{
public:

    TCPListener(int port, boost::asio::io_context & io_context,
    std::function<void(const int, tcp::socket)> onClientConnected) :
        _placeholder(io_context),
        _acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
        _onClientConnected(std::move(onClientConnected))
    {
        _id = 0;
    }

    ~TCPListener() = default;

    void run()
    {
        accept();
    }

private:

    void accept()
    {
        _acceptor.async_accept(_placeholder, boost::bind(&TCPListener::onAccept, this, boost::asio::placeholders::error));
    }

    void onAccept(const boost::system::error_code &error)
    {
        if(error)
        {
            onError(error);
            return;
        }
        _onClientConnected(++_id, std::move(_placeholder));
        accept();
    }

    void onError(const boost::system::error_code &error)
    {
        std::cout << "*** The following exception has been thrown " << error.message() << " ***" << std::endl;
    }

    std::size_t _id;
    tcp::acceptor _acceptor;
    tcp::socket _placeholder;
    std::function<void(const int, tcp::socket)> _onClientConnected;
};