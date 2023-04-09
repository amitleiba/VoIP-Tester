#pragma once

#include "Database.hpp"

class VTDatabase : public Database
{
public:
    VTDatabase(const std::string &domain):
        Database(domain)
    {     
    }

    void saveLog(const bsoncxx::document::value &document)
    {
        save(TEST_LOGS_COLLECTION ,document);
    }

    bsoncxx::document::value getTestLogsHistory()
    {
        mongocxx::collection collection = _database[TEST_LOGS_COLLECTION];
        mongocxx::cursor cursor = collection.find({});

        bsoncxx::builder::stream::document result{};
        bsoncxx::builder::stream::array headers{};

        for (const auto & doc : cursor) 
        {
            bsoncxx::builder::stream::document headerBuilder{};
            headerBuilder << ID << doc[ID].get_oid().value.to_string();

            if(doc[CREATION_TIME])
            {
                headerBuilder << CREATION_TIME << doc[CREATION_TIME].get_string().value.to_string();
            }

            headers << headerBuilder;
        }

        result << HEADERS << headers;

        return result.extract();
    }

    bsoncxx::stdx::optional<bsoncxx::document::value> getLog(std::string id)
    {
        return getDocument(TEST_LOGS_COLLECTION, std::move(id));
    }

private:
    static constexpr auto TEST_LOGS_COLLECTION = "Test-Logs";
    const std::string CREATION_TIME = "creation-time";
    const std::string HEADERS = "headers";
};