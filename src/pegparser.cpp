#include <iostream>
#include <peglib.h>

int main()
{
    std::cout << "Hello Catgirl \n";
}

/**
 * @brief 
 * 
 My current goals
Parse: int main() {}
Parse: int main(){ return 0; }
Parse: int main(){ ; ; return 0; } (empty statements + return)
Token rules (identifiers, keywords, literals, operators, comments, whitespace)

CompoundStatement, If/While/For, Return, ;, expression statements

Expression precedence ladder (this part is easy for you)

THEN add declarations with DeclStart guard

THEN typedef tracking

PEG parser for C bootstrap plan.
 */