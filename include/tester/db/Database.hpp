#pragma once

#include <string>
#include <optional>
#include <vector>
#include <sstream>
#include <typeinfo>

#include "mongocxx/instance.hpp"
#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/json.hpp"
#include "bsoncxx/oid.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/database.hpp"
#include "mongocxx/uri.hpp"
#include "bsoncxx/builder/stream/array.hpp"

class Database
{
public:
    Database(const std::string &domain):
        _instance{},
        _uri(mongocxx::uri(MONGO + domain + PORT)),
        _client(mongocxx::client(_uri)),
        _database(_client[DATABASE_NAME])
    {     
    }

    ~Database() = default;

    void save(std::string collectionName ,const bsoncxx::document::value &document)
    {
        mongocxx::collection collection = _database[collectionName];
        collection.insert_one(document.view());
    }

    bsoncxx::document::value getAllHeaders(std::string collectionName)
    {
        mongocxx::collection collection = _database[collectionName];
        mongocxx::cursor cursor = collection.find({});

        bsoncxx::builder::stream::document result{};
        bsoncxx::builder::stream::array headers{};

        for (auto&& doc : cursor) 
        {
            bsoncxx::builder::stream::document header_builder{};
            header_builder << "_id" << doc["_id"].get_oid().value.to_string();

            if(doc["creation-time"])
            {
                header_builder << "creation-time" << doc["creation-time"].get_string().value.to_string();
            }

            headers << header_builder;
        }

        result << "headers" << headers;

        return result.extract();
    }

    std::vector<bsoncxx::document::value> getAllDocuments(std::string collectionName)
    {
        mongocxx::collection collection = _database[collectionName];
        mongocxx::cursor cursor = collection.find({});

        std::vector<bsoncxx::document::value> results;
        for (auto&& doc : cursor) 
        {
            results.emplace_back(std::move(doc));
        }

        return std::move(results);
    }

    bsoncxx::stdx::optional<std::string> getField(std::string collectionName, std::string id, std::string field_name)
    {
        mongocxx::collection collection = _database[collectionName];
        auto result = collection.find_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{id} << bsoncxx::builder::stream::finalize);

        if (result) 
        {
            bsoncxx::document::view view = result->view();
            if (view[field_name]) 
            {
                auto result = view[field_name].get_string().value.to_string();
                std::cout << result << std::endl;
                return result;
            }
        }

        return bsoncxx::stdx::nullopt;
    }



    bsoncxx::stdx::optional<bsoncxx::document::value> getLog(std::string collectionName, std::string id)
    {
        mongocxx::collection collection = _database[collectionName];
        return collection.find_one(bsoncxx::builder::stream::document{} 
            << "_id" << bsoncxx::oid{id} << bsoncxx::builder::stream::finalize);
    }


    bsoncxx::stdx::optional<bsoncxx::document::value> get(std::string collaction_name,  std::string id, std::string time)
    {
        mongocxx::collection collection = _database[collaction_name];
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::oid document_id(id);

        return collection.find_one(bsoncxx::builder::stream::document{} 
            << "time" << time << bsoncxx::builder::stream::finalize);
    }

private:
    mongocxx::instance _instance;
    mongocxx::uri _uri;
    mongocxx::client _client;
    mongocxx::database _database;

    static constexpr auto DATABASE_NAME = "VoIP-Tester-DB";
    static constexpr auto MONGO = "mongodb://";
    static constexpr auto PORT = ":27017";
};