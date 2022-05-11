/*************************************************************************************************
Solbert -- Copyright (c) 2022, Markus Iser, KIT - Karlsruhe Institute of Technology

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **************************************************************************************************/

#include <iostream>
#include "src/util/CNFFormula.h"
#include "lib/ipasir.h"

int main(int argc, char** argv) {
    std::cout << "I am Solbert" << std::endl;

    CNFFormula f = CNFFormula(argv[1]);
    std::cout << f.nVars() << " " << f.nClauses() << std::endl;


    
    std::cout << ipasir_signature() << std::endl;

    auto* s = ipasir_init();

    for (Cl* clause : f) {
        for (Lit lit : *clause) {
            ipasir_add(s, lit);
        }
        ipasir_add(s, 0);
    }

    int ret = ipasir_solve(s);

    if (ret == 10) {
        std::cout << "satisfiable" << std::endl;

        std::vector<int> vals;

        for (unsigned i = 1; i <= f.nVars(); i++) {
            int val = ipasir_val(s, i);
            vals.push_back(val);
            std::cout << val << " ";
        }
        std::cout << std::endl;

        for (int lit : vals) {
            ipasir_add(s, -lit);
        }
        ipasir_add(s, 0);
    }
    else if (ret == 20) {
        std::cout << "unsatisfiable" << std::endl;
    }
    else {
        std::cout << "foo" << std::endl;
    }
    
    return 0;
}
