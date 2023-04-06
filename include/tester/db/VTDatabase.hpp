#pragma once

#include "Database.hpp"

class VTDatabase : Database
{
public:
    VTDatabase(const std::string &domain):
        Database(domain)
    {     
    }

    ~VTDatabase() = default;

    void save(const bsoncxx::document::value &document)
    {
        Database::save(COLLECTION_NAME ,document);
    }

    bsoncxx::document::value getAllHeaders()
    {
        mongocxx::collection collection = _database[COLLECTION_NAME];
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

    std::vector<bsoncxx::document::value> getAll()
    {
        return Database::getAll(COLLECTION_NAME);
    }

    bsoncxx::stdx::optional<std::string> getField(std::string id, std::string field_name)
    {
        return Database::getField(COLLECTION_NAME, std::move(id), std::move(field_name));
    }

    bsoncxx::stdx::optional<bsoncxx::document::value> getDocument(std::string id)
    {
        return Database::getDocument(COLLECTION_NAME, std::move(id));
    }

private:
    static constexpr auto COLLECTION_NAME = "Test-Logs";
};