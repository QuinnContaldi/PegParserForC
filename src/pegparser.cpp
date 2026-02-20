#include <iostream>
#include <cassert>
#include <peglib.h>

int main() {
  peg::parser p(R"(
    Start <- 'Catgirl' End
    End   <- !.
    %whitespace <- [ \t\r\n]*
  )");

  assert(p);

  p.enable_packrat_parsing();

  assert(p.parse("Catgirl"));
  assert(p.parse("   Catgirl"));
  assert(p.parse("Catgirl   "));
  assert(p.parse("\n\t Catgirl \r\n"));

  // Still rejects extra tokens:
  assert(!p.parse("Catgirl meow"));
  assert(!p.parse("SuperCatgirl"));
  std::cout << "OK\n";
}
