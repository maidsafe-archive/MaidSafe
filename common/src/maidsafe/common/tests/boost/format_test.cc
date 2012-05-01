/* Copyright (c) 2009 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>  // NOLINT
#include <iomanip>

#include "boost/format.hpp"

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

#include "maidsafe/common/test.h"

TEST(boost, BEH_BOOST_formattest1) {
  ASSERT_EQ(boost::str(boost::format("  %%  ")), "  %  ")
      << "basic parsing without arguments failed";
  ASSERT_NE(boost::str(boost::format("nothing")), "nothing ")
      << "Basic parsing without arguments Failed";
  // unsure about this one !!
  ASSERT_EQ(boost::str(boost::format("%%  ")), "%  ")
      << "Basic parsing without arguments Failed";
  ASSERT_NE(("  %%") , "  %")
      << "Basic parsing without arguments Failed";
  // and this one !! TODO(dirvine#5#)
  ASSERT_EQ(boost::str(boost::format("  %n  ")), "    ")
      << "Basic parsing without arguments Failed";
  ASSERT_EQ(boost::str(boost::format("%n  ")), "  ")
      << "Basic parsing without arguments Failed";
  ASSERT_EQ(boost::str(boost::format("  %n")), "  ")
      << "Basic parsing without arguments Failed";
  ASSERT_EQ(boost::str(boost::format("%%##%%##%%1 %1%00") % "Escaped OK"),
            "%##%##%1 Escaped OK00") << "Basic parsing Failed";
  ASSERT_EQ(boost::str(boost::format("%%##%#x ##%%1 %s00") % 20 % "Escaped OK"),
            "%##0x14 ##%1 Escaped OK00") << "Basic p-parsing Failed";
}

struct Rational {
  int n, d;
  Rational(int an, int ad) : n(an), d(ad) {}
};

std::ostream& operator<<(std::ostream& os, const Rational& r) {
  os << r.n << "/" << r.d;
  return os;
}



TEST(boost, BEH_BOOST_formattest2) {
  using boost::format;
  using boost::io::group;
  using boost::str;

  Rational r(16, 9);

  std::string s;
  s = str(format("%5%. %5$=6s . %1% format %5%, c'%3% %1% %2%.\n")
    % "le" % "bonheur" % "est" % "trop" % group(std::setfill('_'), "bref"));

  ASSERT_EQ(s, "bref. _bref_ . le format bref, c'est le bonheur.\n")
      << "centered alignement : formatting result incorrect";

  s = str(format("%+8d %-8d\n") % r % r);
  ASSERT_EQ(s, "  +16/+9 16/9    \n")
      << "(user-type) formatting result incorrect";

  s = str(format("[%0+4d %0+8d %-08d]\n") % 8 % r % r);
  ASSERT_EQ(s, "[+008 +0016/+9 16/9    ]\n")
      << "(zero-padded user-type) formatting result incorrect";

  s = str(format("%1%, %20T_ (%|2$5|,%|3$5| )\n") % "98765" % 1326 % 88);
  ASSERT_EQ(s, "98765, _____________ ( 1326,   88 )\n")
      << "(tabulation) formatting result incorrect";
  s = str(format("%s, %|20t|=") % 88);
  ASSERT_EQ(s, "88,                 =")
      << "(tabulation) formatting result incorrect";

  s = str(format("%.2s %8c.\n") % "root" % "user");
  ASSERT_EQ(s, "ro        u.\n") << "(truncation) formatting result incorrect";
  // width in format-string is overridden by setw manipulator :
  s = str(format("%|1$4| %|1$|") % group(std::setfill('0'), std::setw(6), 1));
  ASSERT_EQ(s, "000001 000001") << "width in format VS in argument misbehaved";

  s = str(format("%|=s|") % group(std::setfill('_'), std::setw(6), r));
  ASSERT_EQ(s, "_16/9_") << "width in group context is not handled correctly";

  // options that uses internal alignment : + 0 #
  s = str(format("%+6d %0#6x %s\n")  % 342 % 33 % "ok");
  ASSERT_EQ(s, "  +342 0x0021 ok\n")
      << "(flags +, 0, or #) formatting result incorrect";

  // flags in the format string are not sticky
  // and hex in argument overrrides type-char d (->decimal) :
  s = str(format("%2$#4d %|1$4| %|2$#4| %|3$|") % 101 %
          group(std::setfill('_'), std::hex, 2) % 103);
  ASSERT_EQ(s, "_0x2  101 _0x2 103")
      <<"formatting error. (not-restoring state ?)";

  // flag '0' is tricky.
  // left-align cancels '0':
  s = str(format("%2$0#12X %2$0#-12d %1$0#10d \n") % -20 % 10);
  ASSERT_EQ(s, "0X000000000A 10           -000000020 \n")
      << "formatting error. (flag 0)";
}
