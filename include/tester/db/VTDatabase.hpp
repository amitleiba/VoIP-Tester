#pragma once

#include "Database.hpp"

class VTDatabase : public Database
{
public:
    VTDatabase(const std::string &domain):
        Database(domain, VT_DATABASE_NAME)
    {     
    }

    void saveTestLog(const bsoncxx::document::value &document)
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
            headerBuilder << ID_FIELD << doc[ID_FIELD].get_oid().value.to_string();

            if(doc[CREATION_TIME_FIELD])
            {
                headerBuilder << CREATION_TIME_FIELD << doc[CREATION_TIME_FIELD].get_string().value.to_string();
            }

            headers << headerBuilder;
        }

        result << HISTORY_HEADERS_FIELD << headers;

        return result.extract();
    }

    bsoncxx::stdx::optional<bsoncxx::document::value> getHistoryLog(std::string documentId)
    {
        return getDocument(TEST_LOGS_COLLECTION, std::move(documentId));
    }

private:
    static constexpr auto VT_DATABASE_NAME = "VoIP-Tester-DB";
    static constexpr auto TEST_LOGS_COLLECTION = "Test-Logs";
    const std::string CREATION_TIME_FIELD = "creation-time";
    const std::string HISTORY_HEADERS_FIELD = "history-headers";
};