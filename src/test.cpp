# include <iostream>
# include <fstream>
# include <iomanip>
# include <cctype>
# include <cstring>
# include "LabelTree.h"

using std::find;
using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::flush;
using std::endl;
using std::setw;

int main(int argc, const char* argv[])
{
    LabelTree tree(3, 6, 3);
    SUPV_T labelset[] = {0, 1, 2, 3, 4, 5};

    cout << tree << endl;

    LabelTree::iterator it = tree.begin();
    it->attach_child(labelset, 3);
    it->attach_child(labelset+3, 1);
    it->attach_child(labelset+4, 2);
    ++it;
    it->attach_child(labelset, 1);
    it->attach_child(labelset+1, 2);
    ++it;
    ++it;
    it->attach_child(labelset+4, 1);
    it->attach_child(labelset+5, 1);

    cout << tree << endl;
    return 0;
}