#pragma once

#include <string>

struct SoftphoneArguments
{
    std::string secret;
    std::string domain;
    int timeout; //in seconds
    std::string id;

    SoftphoneArguments(std::string secret, std::string domain, int timeout, std::string id): 
        secret(std::move(secret)), domain(std::move(domain)), timeout(timeout), id(std::move(id))
    {
    }
};
