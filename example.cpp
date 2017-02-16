/*============================================================================
  Copyright (C) 2017 akitsu sanae
  https://github.com/akitsu-sanae/otabek
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

#include <iostream>
#include "otabek.hpp"

OTABEK_LITERAL(test)
OTABEK_LITERAL(hoge)
OTABEK_LITERAL(dayo)

using namespace otabek;

int main() {
    using parser = sequence<
        repeat<
            or_<
                literal<token::test>,
                literal<token::hoge>
            >
        >,
        literal<token::dayo>
        >;

    auto res = parse<parser>("testhogetesthogedayonyan");
    std::cout << res.second << std::endl;
    res = parse<parser>("testhogetesthogedayon");
    std::cout << res.second << std::endl;
}


