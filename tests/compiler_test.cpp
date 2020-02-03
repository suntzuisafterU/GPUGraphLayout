#include <utility>

struct S {};
 
struct C {
  S &  mc2;
  S && mc3;
  C( S& b, S && c ) :  mc2{b}, mc3{std::move(c)} {} // If this line does not compile, it is an issue with your compiler. Try upgrading.
};
 
int main() { return 0; }
