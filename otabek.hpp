/*============================================================================
  Copyright (C) 2017 akitsu sanae
  https://github.com/akitsu-sanae/otabek
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

#ifndef OTABEK_HPP
#define OTABEK_HPP

#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <sstream>

#define OTABEK_LITERAL(x) \
    namespace otabek { namespace token { \
        constexpr char x[] = #x; \
    }}

namespace otabek {

template<typename T>
struct result {
    std::unique_ptr<T> data;
};

template<typename T>
auto parse(std::string const& input) {
    std::string str = input;
    auto res = T{}.accept(str);
    return std::make_pair(std::move(res), str);
}

namespace detail {

template<typename>
struct mem_fun_ptr_to_result_t;

template<typename T, typename U>
    struct mem_fun_ptr_to_result_t<result<T> (U::*)(std::string&)> {
        using type = T;
    };

}

template<const char* STR>
struct literal {
    result<std::string> accept(std::string& input) {
        std::string value = STR;
        if (value == input.substr(0, value.length())) {
            input.erase(0, value.length());
            return result<std::string> {
                std::make_unique<std::string>(value)
            };
        } else {
            return result<std::string> {
                nullptr
            };
        }
    }
};

struct number {
    result<int> accept(std::string& input) {
        auto it = input.begin();
        for (it; it<input.end(); it++) {
            if (*it < '0' || '9' < *it)
                break;
        }
        if (it == input.begin())
            return result<int> { nullptr };
        auto n = std::stoi(input.substr(0, it - input.begin()));
        input.erase(input.begin(), it);
        return result<int> {
            std::make_unique<int>(n)
        };
    }
};

template<typename ... Ts>
struct sequence {
    using result_t = std::tuple<
        result<typename detail::mem_fun_ptr_to_result_t<
            decltype(&Ts::accept)
        >::type> ...>;

    result<result_t> accept(std::string& input) {
        return result<result_t> {
            std::make_unique<result_t>(Ts{}.accept(input) ...)
        };
    }
};

template<typename Then, typename Else>
struct or_ {
    decltype(parse<Then>("").first) accept(std::string& input) {
        auto res = Then{}.accept(input);
        if (res.data)
            return res;
        return Else{}.accept(input);
    }
};

template<typename P>
struct repeat {
    using data_t = typename detail::mem_fun_ptr_to_result_t<decltype(&P::accept)>::type;
    using result_t = std::vector<data_t>;

    result<result_t> accept(std::string& input) {
        result_t res;
        while (true) {
            auto tmp = P{}.accept(input);
            if (!tmp.data)
                break;
            res.push_back(*tmp.data);
        }
        return result<result_t> {
            std::make_unique<result_t>(res)
        };
    }
};

}

#endif
