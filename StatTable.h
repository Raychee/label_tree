#ifndef _STATTABLE_H
#define _STATTABLE_H

#include <iostream>
#include <iomanip>
#include "List.h"

using std::ostream;
using std::setw;
using std::endl;

template <typename _L> class StatTable {
public:
	StatTable() {}
	StatTable(_L* label, long s_label);
	~StatTable();
	const StatTable<_L>& init(_L* label, long s_label);
	long length() const;
	long num(long i) const;
	_L label(long i) const;
	long* operator[](long i) const;
private:
	long length_;
	_L* label_;
	long* num_;
	long** idx;
};

template <typename _L>
ostream& operator<<(ostream& out, const StatTable<_L>& table);

typedef StatTable<double> Stat;

/***** Class StatTable Function Definition *****/
template <typename _L>
StatTable<_L>::StatTable(_L* label, long s_label) { init(label, s_label); }

template <typename _L>
StatTable<_L>::~StatTable() {
	for (int i = 0; i < length_; i++) {
		delete[] idx[i];
	}
	delete[] idx;
	delete[] num_;
	delete[] label_;
	cout << "Destructor : StatTable" << endl;
}

template <typename _L>
const StatTable<_L>& StatTable<_L>::init(_L* label, long s_label) {
/*
	Parameters:
		label:			Label of every sample
		s_label:		Number of samples (sizeof(label))
*/
	long i, j, temp, verbosity = 1;
	ListNode<List<_L, long>*>* stat;
	List<_L, long>* statlist;
	ListNode<long>* statentry;
	List<char, List<_L, long>*> labelstat(0, new List<_L, long>(label[0], 0));
	
	/* Count the total number of labels */
	if (verbosity >= 1) {
		cout << "Calculating statistics of labels..." << endl;
	}
	for (i = 1; i < s_label; i++) {
		stat = labelstat.head();
		while (stat) {
			statlist = stat->data();
			if (fabs(statlist->label() - label[i]) < 1e-8) {
				statlist->add(i);
				break;
			}
			stat = stat->next();
		}
		if (stat) continue;
		statlist = new List<_L, long>(label[i], i);
		labelstat.add(statlist);
	}
	/* Convert the statistics into table */
	length_ = labelstat.length();
	label_ = new _L[length_];
	num_ = new long[length_];
	idx = new long*[length_];
	stat = labelstat.head();
	for (i = 0; i < length_ && stat; i++) {
		statlist = stat->data();
		label_[i] = statlist->label();
		num_[i] = temp = statlist->length();
		idx[i] = new long[temp];
		statentry = statlist->head();
		for (j = 0; j < temp && statentry; j++) {
			idx[i][j] = statentry->data();
			statentry = statentry->next();
		}
		stat = stat->next();
	}
	if (verbosity >= 1) {
		cout << "Done.\nTotal number of labels: " << length_ << endl;
	}
	return *this;
}

template <typename _L>
inline long StatTable<_L>::length() const { return length_; }

template <typename _L>
inline long StatTable<_L>::num(long i) const { return num_[i]; }

template <typename _L>
inline _L StatTable<_L>::label(long i) const { return label_[i]; }

template <typename _L>
inline long* StatTable<_L>::operator[](long i) const { return idx[i]; }


template <typename _L>
ostream& operator<<(ostream& out, const StatTable<_L>& table) {
	long length = table.length();
	long num, * idx;
	out << "Total number of labels: " << length << endl;
	for (int i = 0; i < length; i++) {
		num = table.num(i);
		idx = table[i];
		out << setw(3) << table.label(i) << " (" << setw(3) << num << ")\t"
			<< setw(3) << idx[0];
		for (int j = 1; j < num; j++) {
			out << " - " << setw(3) << idx[j];
		}
		out << endl;
	}
	return out;
}

#endif