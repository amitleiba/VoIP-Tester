#pragma once

#include <string>
#include <optional>
#include <vector>
#include <sstream>

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
    Database(const std::string &domain, std::string databaseName):
        _instance{},
        _uri(mongocxx::uri(MONGO + domain + PORT)),
        _client(mongocxx::client(_uri)),
        _database(_client[std::move(databaseName)])
    {     
    }

    ~Database() = default;

    void save(std::string collectionName ,const bsoncxx::document::value &document)
    {
        mongocxx::collection collection = _database[std::move(collectionName)];
        collection.insert_one(document.view());
    }

    std::vector<bsoncxx::document::value> getAll(std::string collectionName)
    {
        mongocxx::cursor cursor = _database[std::move(collectionName)].find({});

        std::vector<bsoncxx::document::value> results;
        for (const auto & doc : cursor) 
        {
            results.emplace_back(std::move(doc));
        }

        return std::move(results);
    }

    bsoncxx::stdx::optional<std::string> getField(std::string collectionName, std::string documentId,
        const std::string &field)
    {
        mongocxx::collection collection = _database[std::move(collectionName)];
        auto result = collection.find_one(bsoncxx::builder::stream::document{} << ID_FIELD 
            << bsoncxx::oid{std::move(documentId)} << bsoncxx::builder::stream::finalize);

        if (result) 
        {
            bsoncxx::document::view view = result->view();
            if (view[field]) 
            {
                auto result = view[field].get_string().value.to_string();
                std::cout << result << std::endl;
                return result;
            }
        }

        return bsoncxx::stdx::nullopt;
    }

    bsoncxx::stdx::optional<bsoncxx::document::value> getDocument(std::string collectionName, std::string documentId)
    {
        mongocxx::collection collection = _database[std::move(collectionName)];
        return collection.find_one(bsoncxx::builder::stream::document{} 
            << ID_FIELD << bsoncxx::oid{std::move(documentId)} << bsoncxx::builder::stream::finalize);
    }

protected:
    mongocxx::instance _instance;
    mongocxx::uri _uri;
    mongocxx::client _client;
    mongocxx::database _database;

    static constexpr auto MONGO = "mongodb://";
    static constexpr auto PORT = ":27017";

    const std::string ID_FIELD = "_id";
};