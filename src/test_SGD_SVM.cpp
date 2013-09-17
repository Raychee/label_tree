# include <iostream>
# include <fstream>
# include <iomanip>
# include <cctype>
# include <cstring>
# include "SGD_SVM.h"

using std::find;
using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::flush;
using std::endl;
using std::setw;

void read_samples(const char* file_name,
                  COMP_T*& x, DAT_DIM_T& d, N_DAT_T& n, SUPV_T*& y);

int verbosity = 2;

int main(int argc, const char * argv[])
{
    COMP_T* x; DAT_DIM_T d; N_DAT_T n; SUPV_T* y;

    read_samples(argv[1], x, d, n, y);

    cout << "n = " << n << endl;
    cout << "d = " << d << endl;
    cout << "[ Y X' ] = \n";
    for (N_DAT_T i = 0; i < n; ++i) {
        y[i] = y[i] > 0 ? 1 : -1; 
        cout << y[i] << "\t| ";
        for (DAT_DIM_T j = 0; j < d; ++j) {
            cout << setw(16) << x[i * d + j];
        }
        cout << endl;
    }

    SGD_SVM svm(d, 0.2, 1, 2);
    svm.train(x, d, n, y);

    cout << svm << endl;

    cout << "Write to file? ";
    char yes_no;
    cin >> yes_no;
    if (yes_no == 'y') {
        ofstream file("../data/model.txt");
        file << svm << endl;
        file.close();
    }
    cout << "Done" << endl;

    delete[] x;
    delete[] y;
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
            --y[i_line];
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
