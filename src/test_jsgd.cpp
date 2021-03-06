# include <iostream>
# include <fstream>
# include <iomanip>
# include <cctype>
# include <cstring>

extern "C" {

# include "jsgd/jsgd.h"

}

using std::find;
using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::flush;
using std::endl;
using std::setw;

void read_samples(const char* file_name, x_matrix_t*& x, int*& y);

int verbosity = 2;

int main(int argc, char const *argv[])
{
	x_matrix_t* x;
	int* y;
	float* w, * bias;
	jsgd_params_t param;

	read_samples(argv[1], x, y);
	long n_class = 3;

	long n = x->n;
	long d = x->d;
	cout << "n = " << n << endl;
	cout << "d = " << d << endl;
	cout << "[ Y X' ] = \n";
	for (long i = 0; i < n; ++i) {
		cout << y[i] << "\t| ";
		for (long j = 0; j < d; ++j) {
			cout << setw(16) << x->data[i * d + j];
		}
		cout << endl;
	}

	jsgd_params_set_default(&param);
	
	w = new float[d * n_class];
	bias = new float[n_class];
	jsgd_train(n_class, x, y, w, bias, &param);

	cout << "[W b] = \n";
	for (long i = 0; i < n_class; ++i) {
		for (long j = 0; j < d; ++j) {
			cout << setw(16) << w[i * d + j];
		}
		cout << "\t" << bias[i] << endl;
	}

	cout << "Accuracy = ";
	float correct = 0;
	float* score = new float[n_class];
	for (long i = 0; i < n; ++i) {
		memset(score, 0, sizeof(float) * n_class);
		long y_i = y[i];
		float* x_i = x->data + d * i;
		for (long k = 0; k < n_class; ++k) {
			float* w_k = w + d * k;
			for (long j = 0; j < d; ++j) {
				score[k] += w_k[j] * x_i[j];
			}
			score[k] += bias[k];
		}
		float cur_max = score[0];
		long cur_max_i = 0;
		for (long j = 1; j < n_class; ++j) {
			if (score[j] > cur_max) {
				cur_max = score[j];
				cur_max_i = j;
			}
		}
		if (cur_max_i == y_i) ++correct;
	}
	cout << correct / n << " ( " << correct << "/" << n << " )" << endl;

	cout << "Write to file? ";
	char yes_no;
	cin >> yes_no;
	if (yes_no == 'y') {
		ofstream file("data/model.txt");
		for (long i = 0; i < n_class; ++i) {
			for (long j = 0; j < d; ++j) {
				file << setw(16) << w[i * d + j];
			}
			file << setw(16) << bias[i] << "\n";
		}
		file.close();
	}
	cout << "Done" << endl;

	delete[] x->data;
	delete[] y;
	delete[] x;
	delete[] w;
	delete[] bias;
	delete[] score;
	// int a;
	// int b;
	// cout << (bool)(cin >> a) << endl;
	// cout << (bool)(cin >> b) << endl;
	// cout << "cin.good = " << cin.good() << endl;
	// cout << "cin.eof = " << cin.eof() << endl;
	// cout << "cin.bad = " << cin.bad() << endl;
	// cout << "cin.fail = " << cin.fail() << endl;
	// cout << "a = " << a << endl;
	// cout << "b = " << b << endl;
	// cout << "cin.peek = " << (char)cin.peek()
	// 	 << "(" << cin.peek() << ")" << endl;
	// cout << "cin.good = " << cin.good() << endl;
	// cout << "cin.eof = " << cin.eof() << endl;
	// cout << "cin.bad = " << cin.bad() << endl;
	// cout << "cin.fail = " << cin.fail() << endl;
	return 0;
}

void read_samples(const char* file_name, x_matrix_t*& x, int*& y) {
	if (verbosity >= 1) {
		cout << "Scanning examples..." << flush;
	}
	ifstream file(file_name);
	if (!file.is_open()) {
		cerr << "Failed opening file: " << file_name << endl;
		exit(1);
	}

	long n_lines = 0, d = 0;
	char line[1024];
	while (file.getline(line, 1024)) {
		if (isdigit(line[0])) n_lines++;
		else continue;		// not a valid line that contains data
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
		cout << "Done.\nTotal samples: " << n_lines
			 << ", Feature dimension: " << d
			 << "\nReading samples..." << flush;
	}

	x = new x_matrix_t;
	y = new int[n_lines];
	x->encoding = x_matrix_t::JSGD_X_FULL;
	x->n = n_lines;
	x->d = d;
	x->data = new float[d * n_lines];
	memset(x->data, 0, sizeof(float) * d * n_lines);

	file.clear();
	file.seekg(0);

	long i_line = 0;
	long i_d;
	while (file && i_line < n_lines) {
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
					!(file >> x->data[d * i_line + i_d - 1])) {
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
