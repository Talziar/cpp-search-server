#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

constexpr int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result >> ws;
    return result;
}

vector<string> SplitIntoWords(const string &text) {
    vector<string> words;
    string word;
    for (const char c : (text + " "s)) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
    public:
    void SetStopWords(const string &text) {
        for (const string &word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string &document) {
        vector<string> document_words = SplitIntoWordsNoStop(document);
        int doc_size = static_cast<int>(document_words.size());
        for (const string &word : document_words) {
            word_to_document_freqs_[word][document_id] += 1./doc_size;
        }
        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string &raw_query) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document &lhs, const Document &rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    private:
    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    int document_count_ = 0;

    map<string, map<int, double>> word_to_document_freqs_;

    set<string> stop_words_;

    bool IsStopWord(const string &word) const {
        return stop_words_.count(word) > 0;
    }

    static bool IsMinusWord(const string &word) { 
        return word.front() == '-'; 
    }

    vector<string> SplitIntoWordsNoStop(const string &text) const {
        vector<string> words;
        for (const string &word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string &text) const {
        Query query;
        for (const string &word : SplitIntoWords(text)) {
            if (IsMinusWord(word)) {
                query.minus_words.insert(word.substr(1));
            } else if (!IsStopWord(word)) {
                query.plus_words.insert(word);
            }
        }
        return query;
    }

    vector<Document> FindAllDocuments(const Query &query) const {
        vector<Document> found_documents;
        map<int, double> plus_words_tf_idf;

        for (const string &word : query.plus_words) {
            if (word_to_document_freqs_.contains(word)) {
                double idf = log((document_count_ * 1.) / static_cast<int>(word_to_document_freqs_.at(word).size()));
                for (const auto &[id, tf] : word_to_document_freqs_.at(word)) {
                    plus_words_tf_idf[id] += idf * tf;
                }
            }
        }

        for (const string &word : query.minus_words) {
            if (word_to_document_freqs_.contains(word)) {
                for (const auto &[id, tf] : word_to_document_freqs_.at(word)) {
                    plus_words_tf_idf.erase(id);
                }
            }
        }

        for (const auto &[f, s] : plus_words_tf_idf) {
            found_documents.push_back({f, s});
        }
        return found_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    
    const string query = ReadLine();
    for (const auto &[document_id, relevance] :
         search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}
