#pragma once

#include<iostream>

#include<boost/asio.hpp>
#include<boost/bind.hpp>

class TCPListener
{
public:

    TCPListener(int port, boost::asio::io_context & io_context,
    std::function<void(const int, const boost::asio::ip::tcp::socket &)> onClientConnected) :
        _placeholder(io_context),
        _acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        _onClientConnected(std::move(onClientConnected))
    {
        _id = 0;
    }

    ~TCPListener() = default;

    void accept()
    {
        _acceptor.async_accept(_placeholder, boost::bind(&TCPListener::onAccept, this, boost::asio::placeholders::error));
    }

private:

    void onAccept(const boost::system::error_code &error)
    {
        if(error)
        {
            onError(error);
            return;
        }
        _onClientConnected(++_id, std::move(_placeholder));
    }

    void onError(const boost::system::error_code &error)
    {
        std::cout << "*** The following exception has been thrown" << error.message() << " ***" << std::endl;
    }

    int _id;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _placeholder;
    std::function<void(const int, const boost::asio::ip::tcp::socket &)> _onClientConnected;
};