# include <iostream>
# include <iomanip>
# include <fstream>
# include <cstdio>
# include <cmath>
# include <opencv2/core/core.hpp>
# include "label_tree_common.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::setfill;
using std::setprecision;
using std::snprintf;
using std::ofstream;
using std::sqrt;
using cv::Mat;
using cv::Mat_;
using cv::TermCriteria;

/***** Operations on labels *****/

/***** Machine learning *****/

MODEL** learn_multi_one_vs_rest(MODEL** model, DOC** docs, long s_label,
							   long totwords, LEARN_PARM* learn_parm,
							   KERNEL_PARM* kernel_parm, char* modelfile,
							   char* restartfile, Stat &labelstat) {
/*
	Parameters:
		docs:			Training vectors (x-part)
		s_label:		Number of examples in docs/label
		totwords:		Number of features (i.e. highest feature index)
		learn_parm:		Learning paramenters
		kernel_parm:	Kernel paramenters
		modelfile:		Model file name prefix. No model file output if NULL or 
						empty.
		restartfile:	Restart the optimization from alpha values in this file.
						NULL or empty if not needed.
		i_labelset:		The ith entry of list "i_labelset" stores the list of 
						indexes of samples
						which have label labelset[i]
	Return:
		model:			A set of models of which each is a one-vs-rest
						classifier for one label
 */
	char filename[256];
	double* alpha_in = NULL, * target;
	long s_labelset, k, i, num, * idx;
	KERNEL_CACHE* kernel_cache;

	s_labelset = labelstat.length();
	
	// model = (MODEL **)my_malloc(sizeof(MODEL *) * s_labelset);
	// double *target = (double *)my_malloc(sizeof(double) * s_label);
	target = new double[s_label];

	for (k = 0; k < s_labelset; k++) {
		// model[k] = (MODEL *)my_malloc(sizeof(MODEL));
		if (verbosity >= 1) {
			cout << "=========Learning One-vs-Rest for label " 
				 << k+1 << "/" << s_labelset << "=========" << endl;
		}
		for (i = 0; i < s_label; i++) {
			target[i] = -1;
		}
		num = labelstat.num(k);
		idx = labelstat[k];
		for (i = 0; i < num; i++) {
			target[idx[i]] = 1;
		}
		
		if(kernel_parm->kernel_type == LINEAR) { /* don't need the cache */
			kernel_cache=NULL;
		}
		else {
			/* Always get a new kernel cache. It is not possible to use the
			 same cache for two different training runs */
			kernel_cache = kernel_cache_init(s_label,
											 learn_parm->kernel_cache_size);
		}
		if(restartfile && restartfile[0]) {
			if (snprintf(filename, 256, "%s_%02ld.txt", restartfile, k) > 0) {
				alpha_in = read_alphas(filename, s_label);
			}
			else {
				cerr << "Generating batch of alpha file name: Alpha file \""
					 << restartfile << "_"
					 << setfill('0') << setw(3) << k
					 << ".txt\" is skipped." << endl;
			}
		}
		svm_learn_classification(docs, target, s_label, totwords, learn_parm,
								 kernel_parm, kernel_cache, model[k],alpha_in);
		if(kernel_parm->kernel_type == LINEAR) {
			add_weight_vector_to_linear_model(model[k]);
		}
		else {
			/* Free the memory used for the cache. */
			kernel_cache_cleanup(kernel_cache);
		}

		if (modelfile && modelfile[0]) {
			/* If needed,
			 output the model files for every one-vs-rest classifier */
			if (snprintf(filename, 256, "%s_%02ld.txt", modelfile, k) > 0) {
				write_model(filename, model[k]);
			}
			else {
				cerr << "Generating batch of model file name: Model file \""
					 << modelfile << "_"
					 << setfill('0') << setw(3) << k
					 << ".txt\" is skipped." << endl;
				continue;
			}
		}
	}

	delete[] target;
	return model;
}

long classify_multi_one_vs_rest(MODEL** model, long s_labelset, DOC* ex) {
	double cur, cur_max;
	long i, cur_max_i;
	
	cur = cur_max = classify_example(model[0], ex);
	cur_max_i = 0;
	for (i = 1; i < s_labelset; i++) {
		cur = classify_example(model[i], ex);
		if (cur > cur_max) {
			cur_max = cur;
			cur_max_i = i;
		}
	}
	return cur_max_i;
}

double* get_confusion_matrix(double* confusion_matrix,
							 DOC** docs, Stat &labelstat, MODEL** model,
							 char* confmatfile) {
	long s_labelset, i, k, num, * idx;
	
	s_labelset = labelstat.length();
	for (i = 0; i < s_labelset * s_labelset; i++) {
		confusion_matrix[i] = 0;
	}

	if (verbosity >= 1) {
		cout << "=========Calculating the confusion matrix=========" << endl;
	}

	for (long k = 0; k < s_labelset; k++) {

		if (verbosity >= 1) {
			cout << "Counting the number of samples of label "
				 << labelstat.label(k) << " that is misclassfied..." << flush;
		}

		/* Classify every sample that has the kth label */
		num = labelstat.num(k);
		idx = labelstat[k];
		for (i = 0; i < num; i++) {
			confusion_matrix[ k * s_labelset + 
				classify_multi_one_vs_rest(model, s_labelset, docs[idx[i]])]++;
		}

		if (verbosity >= 1) {
			cout << "Done." << endl;
		}

	}

	if (verbosity >= 1 && s_labelset <= 10) {
		cout << "The matrix is:\n";
		for(i = 0; i < s_labelset; i++) {
			for(k = 0; k < s_labelset; k++) {
				cout << "\t" << setprecision(4)
					 << confusion_matrix[i * s_labelset + k];
			}
			cout << endl;
		}
	}

	/* confusion_matrix = 1/2 * ( C + C' ) */
	if (verbosity >= 1 && s_labelset <= 10) {
		cout << "Calculating the confusion matrix..." << flush;
	}
	for (i = 0; i < s_labelset; i++) {
		for (k = 0; k < i; k++) {
			confusion_matrix[i * s_labelset + k] = 
			confusion_matrix[k * s_labelset + i] =
				(confusion_matrix[i * s_labelset + k] + 
				confusion_matrix[k * s_labelset + i]) / 2;
		}
	}
	if (verbosity >= 1) {
		cout << "Done." << endl;
	}

	if (verbosity >= 1 && s_labelset <= 10) {
		cout << "The confusion matrix is:\n";
		for(i = 0; i < s_labelset; i++) {
			for(k = 0; k < s_labelset; k++) {
				cout << "\t" << setprecision(4)
					 << confusion_matrix[i * s_labelset + k];
			}
			cout << endl;
		}
	}

	if (confmatfile && confmatfile[0]) {
		if (verbosity >= 1) {
			cout << "Writing the confusion matrix to file..." << flush;
		}
		write_confusion_matrix(confmatfile, confusion_matrix, s_labelset);
		if (verbosity >= 1) {
			cout << "Done." << endl;
		}
	}

	return confusion_matrix;
}

long* spectral_clustering(long* cluster, double* W_, long n, long k, 
						  bool* mask) {
/*
	Warning: 
			This implementation is NOT reliable. Due to the restrictions of the 
		function cv::kmeans, The similarity matrix W_ is of type double, but is 
		actually treated as float during calculation. The resulting array 
	 	cluster is likewise treated as int []. Any data value that exceeds 
	 	the range of those arrays' capacities will cause random solutions.

	Parameters:
		cluster:	Output cluster array(size of n)
		W_:			Similarity matrix(row-majored)
		mask:		Mask
		n:			The length of W (W is a n×n matrix)
		k:			The number of clusters to be clustered
	Return:
		cluster:	The cluster label of every sample (an array of length n)
*/
	int i, j;
	double* ptrW;
	float* ptrL;
	float* D_negsq = new float[n];
/*	double **L = (double **)my_malloc(sizeof(double *) * n); */
	Mat_<double> W(n, n, W_);
	Mat_<float> L(n, n);
	Mat_<float> LeigVal, LeigVec, kmDat, kmDatEntry;
	Mat Clabel; // Clabel is actually a Mat_<int> matrix(type CV_32S)

	/* Calculate D */
	for (i = 0; i < n; i++) {
		D_negsq[i] = 0;
		ptrW = W[i];
		for (j = 0; j < n; j++) {
			D_negsq[i] += (float)ptrW[j];
		}
		D_negsq[i] = 1 / sqrt(D_negsq[i]);
	}
	/* Calculate normalized laplacian L = I - D^(-1/2)*W*D(-1/2) */
	for (i = 0; i < n; i++) {
		ptrL = L[i];
		ptrW = W[i];
		for (j = i; j < n; j++) {
			ptrL[j] = - D_negsq[i] * (float)ptrW[j] * D_negsq[j];
			if (j == i) { ptrL[j]++; }
		}
	}
	cv::completeSymm(L);

	/* Calculate the first k eigenvectors of L */
	cv::eigen(L, LeigVal, LeigVec);
	kmDat = LeigVec.rowRange(n - k, n).t();
	/* Normalization */
	for (i = 0; i < n; i++) {
		kmDatEntry = kmDat.row(i);
		cv::normalize(kmDatEntry, kmDatEntry);
	}
	/* K-means */
	cv::kmeans(kmDat, k, Clabel,
			   TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 1e5, 1e-8),
			   10, cv::KMEANS_PP_CENTERS);
	if (Clabel.isContinuous()) {
		int* ptrC = Clabel.ptr<int>(0);
		for (i = 0; i < n; i++) {
			cluster[i] = (long)ptrC[i];
		}
	}
	else {
		for (i = 0; i < n; i++) {
			cluster[i] = (long)Clabel.at<int>(i);
		}
	}

	return cluster;
}


/***** Miscellaneous *****/

void write_confusion_matrix(char* confmatfile, double* confusion_matrix, 
	long s_labelset) {
	
	// FILE *matfl;
	ofstream matfl(confmatfile);
	
	if (verbosity >= 1) {
		cout << "Writing confusion matrix to file..." << flush;
	}

	matfl << s_labelset << " # number of labels\n";
	for(long i = 0; i < s_labelset; i++) {
		for(long k = 0; k < s_labelset; k++) {
			matfl << confusion_matrix[i*  s_labelset + k] << "\t";
		}
		matfl << endl;
	}
	matfl.close();
	if (verbosity >= 1) {
		cout << "Done." << endl;
	}
}

