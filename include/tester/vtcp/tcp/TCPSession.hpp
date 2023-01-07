#pragma once

#include<iostream>

#include<boost/asio.hpp>

class TCPSession
{
public:
    TCPSession(boost::asio::ip::tcp::socket socket) : 
        _socket(std::move(socket))
    {}

    ~TCPSession() = default;

    void start()
    {

    }

private:
    boost::asio::ip::tcp::socket _socket;

};