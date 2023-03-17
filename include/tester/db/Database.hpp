#pragma once

#include <string>
#include <optional>

#include "mongocxx/instance.hpp"
#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/json.hpp"
#include "bsoncxx/oid.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/database.hpp"
#include "mongocxx/uri.hpp"

const std::string uri_str = "mongodb://192.168.1.123:27017";

class Database
{
public:
    Database(const std::string &domain):
        _instance{},
        _uri(mongocxx::uri(uri_str)),
        _client(mongocxx::client(_uri)),
        _database(_client[DATABASE_NAME])
    {     
    }

    ~Database() = default;

    void save(const std::string &collaction_name ,const bsoncxx::document::value &document)
    {
        mongocxx::collection collection = _database[collaction_name];
        collection.insert_one(document.view());
    }

    bsoncxx::stdx::optional<bsoncxx::document::value> get(const std::string &collaction_name, const std::string &id)
    {
        mongocxx::collection collection = _database[collaction_name];
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::oid document_id(id);

        return collection.find_one(bsoncxx::builder::stream::document{} 
            << "_id" << id << bsoncxx::builder::stream::finalize);
    }

    bsoncxx::stdx::optional<bsoncxx::document::value> get(const std::string &collaction_name,  const std::string &id, const std::string &time)
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


    const std::string _domain_uri;
    static constexpr auto DATABASE_NAME = "VoIP-Tester-DB";
    static constexpr auto MONGO = "mongodb://";
    static constexpr auto PORT = ":27017";
};