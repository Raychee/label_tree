# ifndef _LABEL_TREE_COMMON_H
# define _LABEL_TREE_COMMON_H

extern "C" {

# include "svm_light/svm_common.h"
# include "svm_light/svm_learn.h"

}

# include "StatTable.h"
# include "LabelTree.h"

/* Operations on labels */

/* Machine learning */
MODEL** learn_multi_one_vs_rest(MODEL** model, DOC** docs, long s_label,
							   long totwords, LEARN_PARM* learn_parm,
							   KERNEL_PARM* kernel_parm, char* modelfile,
							   char* restartfile, Stat& labelstat);
long classify_multi_one_vs_rest(MODEL** model, long s_labelset, DOC* ex);
double* get_confusion_matrix(double* confusion_matrix,
							 DOC** docs, Stat& labelstat, MODEL** model,
							 char* confmatfile = NULL);
long* spectral_clustering(long* cluster, double* W_, long n, long k);

/***** Miscellaneous *****/
void write_confusion_matrix(char* confmatfile, double* confusion_matrix, 
							long s_labelset);
#endif