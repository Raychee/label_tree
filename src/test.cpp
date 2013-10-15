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
using std::left;
using std::right;

void read_samples(const char* file_name,
                  COMP_T*& x, DAT_DIM_T& d, N_DAT_T& n, SUPV_T*& y);

int verbosity = 3;

int main(int argc, const char* argv[])
{
    if (argc <= 1) {
        cerr << "Not enough parameters." << endl;
        exit(1);
    }


    LabelTree tree(2, 6, 2, 1, verbosity);
    SUPV_T labelset[] = {1, 2, 3, 4, 5, 6};
    COMP_T* x; DAT_DIM_T d; N_DAT_T n; SUPV_T* y;
    ofstream file_2, file_3;

    cout << tree << endl;

    read_samples(argv[1], x, d, n, y);

    cout << "n = " << n << endl;
    cout << "d = " << d << endl;
    cout << "[ Y X' ] = \n";
    for (N_DAT_T i = 0; i < n; ++i) {
        cout << left << setw(6) << y[i] << "|";
        for (DAT_DIM_T j = 0; j < d; ++j) {
            cout << right << setw(12) << x[i * d + j];
        }
        cout << endl;
    }

    LabelTree::iterator it = tree.begin();
    it->attach_child(labelset, 3);
    it->attach_child(labelset+3, 3);
    ++it;
    it->attach_child(labelset, 2);
    it->attach_child(labelset+2, 1);
    ++it;
    it->attach_child(labelset+3, 2);
    it->attach_child(labelset+5, 1);
    ++it;
    it->attach_child(labelset, 1);
    it->attach_child(labelset+1, 1);
    ++it;
    ++it;
    it->attach_child(labelset+3, 1);
    it->attach_child(labelset+4, 1);

    cout << "Obj = " << tree.compute_obj(x, d, n, y) << endl;
    if (argc >= 4) {
        file_3.open(argv[3]);
        tree.ostream_of_training_process(file_3);
    }

    tree.train(x, d, n, y);

    file_3.close();

    cout << tree << endl;
    
    if (argc >= 3) {
        cout << "Writing to file " << argv[2] << " ... " << flush;
        file_2.open(argv[2]);
        file_2 << tree << endl;
        file_2.close();
        cout << "Done." << endl;
    }

    COMP_T* x_i            = x;
    N_DAT_T num_of_correct = 0;
    cout << "Testing...\nTruth\tTest\t | Data\n";
    for (N_DAT_T i = 0; i < n; ++i, x_i += d) {
        SUPV_T y_i = tree.test_one(x_i, d);
        cout << setw(8) << y[i] << setw(8) << y_i << " | ";
        for (DAT_DIM_T j = 0; j < d; ++j) {
            cout << setw(16) << x[i * d + j];
        }
        cout << "\n";
        if (y[i] == y_i) {
             num_of_correct++;
        }
    }
    cout << "Accuracy = " << (COMP_T)num_of_correct / n
         << " (" << num_of_correct << "/" << n << ")" << endl;


    // SUPV_T* ran = new SUPV_T[n];
    // tree.rand_data_index(ran, y, n);
    // for (N_DAT_T i = 0; i < n; ++i) {
    //     cout << ran[i] << "\t" << y[ran[i]] << endl;
    // }
    // delete[] ran;
    return 0;
}

void read_samples(const char* file_name,
                  COMP_T*& x, DAT_DIM_T& d, N_DAT_T& n, SUPV_T*& y) {
    if (verbosity >= 1) {
        cout << "Scanning examples..." << flush;
    }
    ifstream file(file_name);
    if (!file.is_open()) {
        cerr << "Failed opening file: " << file_name << endl;
        exit(1);
    }

    n = 0; d = 0;
    char line[1024];
    while (file.getline(line, 1024)) {
        if (isdigit(line[0])) n++;
        else continue;      // not a valid line that contains data
        long cur_d = 0;
        int str_len = file.gcount();

        for (int i = str_len - 1; i >= 0; i--) {
            switch (line[i]) {
                case ':':
                    if (cur_d == 0) {
                        i--;
                        for (int k = 1; i >=0 && isdigit(line[i]); i--, k*=10) {
                            cur_d += k * (line[i] - '0');
                        }
                    }
                    break;
                case '#': cur_d = 0; break;
            }
        }
        d = cur_d > d ? cur_d : d;
    }

    if (verbosity >= 1) {
        cout << "Done.\nTotal samples: " << n
             << ", Feature dimension: " << d
             << "\nReading samples..." << flush;
    }

    x = new COMP_T[d * n];
    y = new SUPV_T[n];
    memset(x, 0, sizeof(COMP_T) * d * n);

    file.clear();
    file.seekg(0);

    N_DAT_T   i_line = 0;
    DAT_DIM_T i_d;
    while (file && i_line < n) {
        char ch;
        if (!(file >> y[i_line])) {
            if (file.eof()) break;
            file.clear();
            switch (file.peek()) {
            case '#': file.ignore(1024, '\n'); continue;
            default: 
                cerr << "Error: Corrputed data file.1" << endl; exit(1);
            }
        }
        else {
            while (file) {
                ch = file.peek();
                if (isdigit(ch)) {
                    file >> i_d;
                }
                else {
                    switch (ch) {
                    case ' ': file.get(); continue;
                    case '#': 
                    case '\n': file.ignore(1024, '\n'); ++i_line; goto newline;
                    default:
                        cerr << "Error: Corrputed data file.2" << endl; exit(1);
                    }
                }
                if (!(file >> ch) || 
                    ch != ':' || 
                    !(file >> x[d * i_line + i_d - 1])) {
                    cerr << "Error: Corrputed data file.3" << endl; exit(1);
                }

            }
        }
      newline:;
    }
    if (verbosity >= 1) {
        cout << "Done." << endl;
    }
    file.close();
}
