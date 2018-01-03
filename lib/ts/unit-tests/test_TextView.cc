/** @file

    TextView unit tests.

    @section license License

    Licensed to the Apache Software Foundation (ASF) under one
    or more contributor license agreements.  See the NOTICE file
    distributed with this work for additional information
    regarding copyright ownership.  The ASF licenses this file
    to you under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in compliance
    with the License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <ts/TextView.h>
#include <catch.hpp>
#include <iostream>

TEST_CASE("TextView Constructor", "[libts][TextView]")
{
  static std::string base = "Evil Dave Rulez!";
  ts::TextView tv(base);
  ts::TextView a{"Evil Dave Rulez"};
  ts::TextView b{base.data(), base.size()};
  ts::TextView c{ts::string_view(base)};
  constexpr ts::TextView d{"Grigor!"_sv};
}

TEST_CASE("TextView Operations", "[libts][TextView]")
{
  ts::TextView tv{"Evil Dave Rulez"};
  size_t off;

  REQUIRE(tv.find('l') == 3);
  off = tv.find_if([](char c) { return c == 'D'; });
  REQUIRE(off == tv.find('D'));
}

TEST_CASE("TextView Trimming", "[libts][TextView]")
{
  ts::TextView tv("  Evil Dave Rulz   ...");
  ts::TextView tv2{"More Text1234567890"};
  REQUIRE("Evil Dave Rulz   ..." == ts::TextView(tv).ltrim_if(&isspace));
  REQUIRE(tv2 == ts::TextView{tv2}.ltrim_if(&isspace));
  REQUIRE("More Text" == ts::TextView{tv2}.rtrim_if(&isdigit));
  REQUIRE("  Evil Dave Rulz   " == ts::TextView(tv).rtrim('.'));
  REQUIRE("Evil Dave Rulz" == ts::TextView(tv).trim(" ."));
}

TEST_CASE("TextView Affixes", "[libts][TextView]")
{
  ts::TextView tv1("0123456789;01234567890");
  ts::TextView prefix{tv1.prefix(10)};

  REQUIRE("0123456789" == prefix);
  REQUIRE("67890" == tv1.suffix(5));

  ts::TextView tv2 = tv1.prefix(';');
  REQUIRE(tv2 == "0123456789");

  ts::TextView right{tv1};
  ts::TextView left{right.split_prefix_at(';')};
  REQUIRE(right.size() == 11);
  REQUIRE(left.size() == 10);

  ts::TextView tv3 = "abcdefg:gfedcba";
  left             = tv3;
  right            = left.split_suffix_at(";:,");
  ts::TextView pre{tv3}, post{pre.split_suffix_at(7)};
  REQUIRE(right.size() == 7);
  REQUIRE(left.size() == 7);
  REQUIRE(left == "abcdefg");
  REQUIRE(right == "gfedcba");

  ts::TextView addr1{"[fe80::fc54:ff:fe60:d886]"};
  ts::TextView addr2{"[fe80::fc54:ff:fe60:d886]:956"};
  ts::TextView addr3{"192.168.1.1:5050"};

  ts::TextView t = addr1;
  ++t;
  REQUIRE("fe80::fc54:ff:fe60:d886]" == t);
  ts::TextView a = t.take_prefix_at(']');
  REQUIRE("fe80::fc54:ff:fe60:d886" == a);
  REQUIRE(t.empty());

  t = addr2;
  ++t;
  a = t.take_prefix_at(']');
  REQUIRE("fe80::fc54:ff:fe60:d886" == a);
  REQUIRE(':' == *t);
  ++t;
  REQUIRE("956" == t);

  t = addr3;
  ts::TextView s{t.suffix(':')};
  REQUIRE("5050" == s);
  REQUIRE(t == addr3);
}

TEST_CASE("TextView Formatting", "[libts][TextView]")
{
  ts::TextView a("01234567");
  {
    std::ostringstream buff;
    buff << '|' << a << '|';
    REQUIRE(buff.str() == "|01234567|");
  }
  {
    std::ostringstream buff;
    buff << '|' << std::setw(5) << a << '|';
    REQUIRE(buff.str() == "|01234567|");
  }
  {
    std::ostringstream buff;
    buff << '|' << std::setw(12) << a << '|';
    REQUIRE(buff.str() == "|    01234567|");
  }
  {
    std::ostringstream buff;
    buff << '|' << std::setw(12) << std::right << a << '|';
    REQUIRE(buff.str() == "|    01234567|");
  }
  {
    std::ostringstream buff;
    buff << '|' << std::setw(12) << std::left << a << '|';
    REQUIRE(buff.str() == "|01234567    |");
  }
  {
    std::ostringstream buff;
    buff << '|' << std::setw(12) << std::right << std::setfill('_') << a << '|';
    REQUIRE(buff.str() == "|____01234567|");
  }
  {
    std::ostringstream buff;
    buff << '|' << std::setw(12) << std::left << std::setfill('_') << a << '|';
    REQUIRE(buff.str() == "|01234567____|");
  }
}
