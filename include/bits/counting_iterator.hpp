#pragma once

#include <vector>

#include "integer_types.hpp"

namespace jada {


struct counting_iterator {

private:
    using self = counting_iterator;

public:
    using value_type        = index_type;
    using difference_type   = index_type; //std::ptrdiff_t;
    using pointer           = index_type*;
    using reference         = index_type&;
    using iterator_category = std::random_access_iterator_tag;

    counting_iterator()
        : value(0) {}
    explicit counting_iterator(value_type v)
        : value(v) {}

    value_type operator*() const { return value; }
    value_type operator[](difference_type n) const { return value + n; }

    self& operator++() {
        ++value;
        return *this;
    }
    self operator++(int) {
        self result{value};
        ++value;
        return result;
    }
    self& operator--() {
        --value;
        return *this;
    }
    self operator--(int) {
        self result{value};
        --value;
        return result;
    }
    self& operator+=(difference_type n) {
        value += n;
        return *this;
    }
    self& operator-=(difference_type n) {
        value -= n;
        return *this;
    }

    friend self operator+(self const& i, difference_type n) {
        return self(i.value + n);
    }
    friend self operator+(difference_type n, self const& i) {
        return self(i.value + n);
    }
    friend difference_type operator-(self const& x, self const& y) {
        return x.value - y.value;
    }
    friend self operator-(self const& i, difference_type n) {
        return self(i.value - n);
    }

    friend bool operator==(self const& x, self const& y) {
        return x.value == y.value;
    }
    friend bool operator!=(self const& x, self const& y) {
        return x.value != y.value;
    }
    friend bool operator<(self const& x, self const& y) {
        return x.value < y.value;
    }
    friend bool operator<=(self const& x, self const& y) {
        return x.value <= y.value;
    }
    friend bool operator>(self const& x, self const& y) {
        return x.value > y.value;
    }
    friend bool operator>=(self const& x, self const& y) {
        return x.value >= y.value;
    }

private:
    value_type value;
};

} // namespace jada