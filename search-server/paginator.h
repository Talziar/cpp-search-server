#pragma once

#include <vector>
#include <iostream>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end), page_size_(distance(begin, end)) {}
    
    Iterator begin() const { return begin_; }
    Iterator end() const { return end_; }
    size_t size() const { return page_size_; }
    
private:
    Iterator begin_;
    Iterator end_;
    size_t page_size_;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& output, const IteratorRange<Iterator>& iterator_range) {
    for (auto iter = iterator_range.begin(); iter != iterator_range.end(); ++iter) {
        output << *iter;
    }
    return output;
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator range_begin, Iterator range_end, size_t page_size) {
        size_t pages_count = distance(range_begin, range_end) / page_size;
        Iterator cur_begin = range_begin;
        while (pages_count --> 0) {
            pages_.emplace_back(cur_begin, next(cur_begin, page_size));
            advance(cur_begin, page_size);
        }
        if (cur_begin != range_end) {
             pages_.emplace_back(cur_begin, range_end);
        }
    }
    
    auto begin() const { return pages_.begin(); }
    auto end() const { return pages_.end(); }
    size_t size() const { return pages_.size(); }
private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}