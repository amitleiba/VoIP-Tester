#pragma once

#include <string>

struct SoftphoneArguments
{
    std::string secret;
    std::string domain;
    int timeout; //in seconds
    int id;

    SoftphoneArguments(std::string secret, std::string domain, int timeout, int id): 
        secret(std::move(secret)), domain(std::move(domain)), timeout(timeout), id(id)
    {
    }
};
