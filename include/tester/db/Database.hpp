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

    // bsoncxx::stdx::optional<bsoncxx::document::value> get(const std::string &collaction_name, const std::string &id)
    // {
    //     mongocxx::collection collection = _database[collaction_name];
    //     auto builder = bsoncxx::builder::stream::document{};
    //     bsoncxx::oid document_id(id);

    //     return collection.find_one(bsoncxx::builder::stream::document{} 
    //         << "_id" << id << bsoncxx::builder::stream::finalize);
    // }

    // std::vector<std::string> get_all_headers(const std::string& collection_name)
    // {
    //     std::vector<bsoncxx::document::value> documents = get_all_documents(collection_name);
    //     std::vector<std::string> results;

    //     bsoncxx::builder::stream::array result_array{};

    //     // for (const auto& document : documents) {
    //     //     bsoncxx::document::view view = document.view();
    //     //     std::stringstream header;
    //     //     // header << "_id: " << view["_id"].get_string().value.to_string() << "\ncreation-time: " << view["creation-time"].get_string().value.to_string();
    //     //     header << "_id: " << view["_id"].get_string().value.to_string();
    //     //     results.push_back(header.str());
    //     // }

    //     for (const auto& doc_value : documents) {
    //         bsoncxx::document::view doc = doc_value.view();

    //         auto id_value = doc["_id"];
    //         auto creation_time_value = doc["creation-time"];

    //         // Check if both "_id" and "creation-time" fields are present in the document
    //         if (!id_value || !creation_time_value) {
    //             continue;
    //         }

    //         bsoncxx::builder::stream::document id_doc;
    //         id_doc << "_id" << id_value.get_oid();
    //         result_array << id_doc;

    //         bsoncxx::builder::stream::document creation_time_doc;
    //         creation_time_doc << "creation-time" << creation_time_value.get_date();
    //         result_array << creation_time_doc;
    //     }
    //     // return results;
    //     return bsoncxx::document::value{bsoncxx::builder::stream::open_array << result_array << bsoncxx::builder::stream::close_array << bsoncxx::builder::stream::finalize};
    // }

    // std::vector<bsoncxx::document::value> get_all_headers(const std::string& collection_name)
    // {
    //     mongocxx::collection collection = _database[collection_name];
    //     mongocxx::cursor cursor = collection.find({});

    //     std::vector<bsoncxx::document::value> results;
    //     bsoncxx::builder::stream::array result{};
    //     for (auto&& doc : cursor) {
    //         result << bsoncxx::builder::stream::open_document;
    //         if(doc["_id"])
    //         {
    //             // std::cout << doc["_id"].get_oid().value.to_string() <<std::endl;
    //             result << "_id" << doc["_id"].get_oid().value.to_string();
    //         }

    //         if(doc["creation-time"])
    //         {
    //             // std::cout << doc["creation-time"].get_string().value.to_string() <<std::endl;
    //             result << "creation-time" << doc["creation-time"].get_string().value.to_string();
    //         }   
            
    //         result << bsoncxx::builder::stream::close_document;
    //         // std::cout << bsoncxx::to_json(result.view());

    //         // results.emplace_back(std::move(doc));

    //     }

    //     return result;
    // }
    bsoncxx::document::value getAllHeaders(const std::string& collection_name)
    {
        mongocxx::collection collection = _database[collection_name];
        mongocxx::cursor cursor = collection.find({});

        bsoncxx::builder::stream::document result{};
        bsoncxx::builder::stream::array headers{};

        for (auto&& doc : cursor) {
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

    std::vector<bsoncxx::document::value> get_all_documents(const std::string& collection_name)
    {
        mongocxx::collection collection = _database[collection_name];
        mongocxx::cursor cursor = collection.find({});

        std::vector<bsoncxx::document::value> results;
        for (auto&& doc : cursor) {
            results.emplace_back(std::move(doc));
        }

        return results;
    }

    bsoncxx::stdx::optional<std::string> get_field(const std::string& collection_name, const std::string& id, const std::string& field_name)
    {
        mongocxx::collection collection = _database[collection_name];
        bsoncxx::stdx::optional<bsoncxx::document::value> result = collection.find_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{id} << bsoncxx::builder::stream::finalize);

        if (result) {
            bsoncxx::document::view view = (*result).view();
            if (view[field_name]) {
                std::cout << std::string{ view[field_name].get_string().value.to_string() } << std::endl;
                return std::string{ view[field_name].get_string().value.to_string() };
            }
        }

        return bsoncxx::stdx::nullopt;
    }



    bsoncxx::stdx::optional<bsoncxx::document::value> getLog(const std::string& collection_name, const std::string& id)
    {
        mongocxx::collection collection = _database[collection_name];
        bsoncxx::stdx::optional<bsoncxx::document::value> result = collection.find_one(bsoncxx::builder::stream::document{} 
            << "_id" << bsoncxx::oid{id} << bsoncxx::builder::stream::finalize);

        return result;
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