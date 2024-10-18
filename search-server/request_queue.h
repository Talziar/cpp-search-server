#pragma once
#include "search_server.h"

#include <deque>
#include <vector>
#include <string>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        const std::vector<Document> documents = search_server_.FindTopDocuments(raw_query, document_predicate);
        QueryResult query_result = {documents, documents.empty()};
        if (requests_.size() == min_in_day_) {
            no_result_requests_ -= (requests_.front().no_result);
            requests_.pop_front();
        }
        no_result_requests_ += query_result.no_result;
        requests_.push_back(query_result);
        return documents;
    }
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    
    int GetNoResultRequests() const;
private:
    struct QueryResult {
        std::vector<Document> result;
        bool no_result;
    };
    
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    int no_result_requests_;
    std::deque<QueryResult> requests_;
};