# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cmath>
# include <iostream>
# include <iomanip>
# include <queue>
# include "label_tree_common.h"
# include "LabelTree.h"

using std::cin;
using std::cout;
using std::endl;
using std::queue;

const int nary = 2;

/*char	predictionsfile[200]; */

void read_input_parameters(int argc,char* argv[],char* docfile,char* modelfile,
						   char* restartfile, char* confmatfile,
						   long* verbosity,
						   LEARN_PARM* learn_parm, KERNEL_PARM* kernel_parm);
void	wait_any_key();
void	print_help();

int main(int argc, char* argv[]) {
	long	i, j, k;
	long	totwords, totdoc, s_labelset, cur_s_labelset, sub_s_labelset;
	long	* cur_labelset, * sub_labelset, * sub_labelset_id, * cluster;
	double* label;
	char	docfile[200];           /* file with training examples */
	char	modelfile[200];         /* file for resulting classifier */
	char	restartfile[200];       /* file with initial alphas */
	char	confmatfile[200];		/* file to store the confusion matrix */
	double* confusion_matrix, * w;
	DOC**	docs;					/* training examples */
	LEARN_PARM learn_parm;
	KERNEL_PARM kernel_parm;
	MODEL** model;
	queue<LabelTreeNode*> pipeline;
	Stat labelstat;
	StatTable<long>* clusterstat;
	LabelTree label_tree;
	LabelTreeNode* parent, * child;

	read_input_parameters(argc, argv, docfile,
						  modelfile, restartfile, confmatfile,
						  &verbosity, &learn_parm, &kernel_parm);
	read_documents(docfile, &docs, &label, &totwords, &totdoc);
	/* Now "label" stores the labels for each doc */

	labelstat.init(label, totdoc);
	cout << labelstat << endl;

	s_labelset = labelstat.length();
	confusion_matrix = new double[s_labelset * s_labelset];
	w = new double[s_labelset * s_labelset];
	cluster = new long[s_labelset];
	model = (MODEL **)my_malloc(sizeof(MODEL *) * s_labelset);
	for (long i = 0; i < s_labelset; i++) {
		model[i] = (MODEL *)my_malloc(sizeof(MODEL) * s_labelset);
	}

	/* Learn one-vs-rest classifier for every label */
	learn_multi_one_vs_rest(model, docs, totdoc, totwords, &learn_parm,
							&kernel_parm, modelfile, restartfile, labelstat);

	/* Generate the confusion matrix */
	get_confusion_matrix(confusion_matrix,
						 docs, labelstat, model, confmatfile);
	
	cur_s_labelset = s_labelset;
	cur_labelset = new long[cur_s_labelset];
	for (i = 0; i < cur_s_labelset; i++) {
		cur_labelset[i] = i;
	}
	label_tree.init(nary, cur_labelset, cur_s_labelset);
	pipeline.push(label_tree.root());
	while (!pipeline.empty()) {
		parent = pipeline.front();
		cur_labelset = parent->labels();
		cur_s_labelset = parent->num_of_labels();
		if (cur_s_labelset <= nary) {
			if (cur_s_labelset > 1) {
				for (i = 0; i < cur_s_labelset; i++) {
					sub_labelset = new long[1];
					sub_labelset[0] = cur_labelset[i];
					child = new LabelTreeNode(nary, sub_labelset, 1);
					parent->attach_child(child);
				}
			}
		}
		else {
			/* construct the sub similarity matrix */
			k = 0;
			for (i = 0; i < cur_s_labelset; i++) {
				for (j = 0; j < cur_s_labelset; j++) {
					w[k++] = confusion_matrix[ cur_labelset[i] * s_labelset 
											 + cur_labelset[j] ];
				}
			}
			spectral_clustering(cluster, w, cur_s_labelset, nary);
			clusterstat = new StatTable<long>(cluster, cur_s_labelset);
			for (i = 0; i < nary; i++) {
				sub_labelset_id = (*clusterstat)[i];
				sub_s_labelset = clusterstat->num(i);
				sub_labelset = new long[sub_s_labelset];
				for (j = 0; j < sub_s_labelset; j++) {
					sub_labelset[j] = cur_labelset[sub_labelset_id[j]];
				}
				child = new LabelTreeNode(nary, sub_labelset, sub_s_labelset);
				parent->attach_child(child);
				pipeline.push(child);
			}
			delete clusterstat;
		}
		pipeline.pop();
	}

	cout << label_tree << endl;

	for (i = 0; i < s_labelset; i++) {
		free_model(model[i], 0);
	}
	for (i = 0; i < totdoc; i++) {
		free_example(docs[i], 1);
	}
	delete[] confusion_matrix;
	delete[] w;
	delete[] cluster;
	free(docs);
	free(label);
	return 0;
}

void read_input_parameters(int argc,char* argv[],char* docfile,char* modelfile,
						   char* restartfile, char* confmatfile,
						   long* verbosity,
						   LEARN_PARM* learn_parm, KERNEL_PARM* kernel_parm)
{
	long i;
	char type[100];

	/* set default */
	set_learning_defaults(learn_parm, kernel_parm);
	strcpy (modelfile, "");
	strcpy (restartfile, "");
	(*verbosity)=1;
	strcpy(type,"c");

	for(i=1;(i<argc) && ((argv[i])[0] == '-');i++) {
		switch ((argv[i])[1])
		{
			case '?': print_help(); exit(0);
			case 'z': i++; strcpy(type,argv[i]); break;
			case 'v': i++; (*verbosity)=atol(argv[i]); break;
			case 'b': i++; learn_parm->biased_hyperplane=atol(argv[i]); break;
			case 'i': i++; learn_parm->remove_inconsistent=atol(argv[i]); break;
			case 'f': i++; learn_parm->skip_final_opt_check=!atol(argv[i]); break;
			case 'q': i++; learn_parm->svm_maxqpsize=atol(argv[i]); break;
			case 'n': i++; learn_parm->svm_newvarsinqp=atol(argv[i]); break;
			case '#': i++; learn_parm->maxiter=atol(argv[i]); break;
			case 'h': i++; learn_parm->svm_iter_to_shrink=atol(argv[i]); break;
			case 'm': i++; learn_parm->kernel_cache_size=atol(argv[i]); break;
			case 'c': i++; learn_parm->svm_c=atof(argv[i]); break;
			case 'w': i++; learn_parm->eps=atof(argv[i]); break;
			case 'p': i++; learn_parm->transduction_posratio=atof(argv[i]); break;
			case 'j': i++; learn_parm->svm_costratio=atof(argv[i]); break;
			case 'e': i++; learn_parm->epsilon_crit=atof(argv[i]); break;
			case 'o': i++; learn_parm->rho=atof(argv[i]); break;
			case 'k': i++; learn_parm->xa_depth=atol(argv[i]); break;
			case 'x': i++; learn_parm->compute_loo=atol(argv[i]); break;
			case 't': i++; kernel_parm->kernel_type=atol(argv[i]); break;
			case 'd': i++; kernel_parm->poly_degree=atol(argv[i]); break;
			case 'g': i++; kernel_parm->rbf_gamma=atof(argv[i]); break;
			case 's': i++; kernel_parm->coef_lin=atof(argv[i]); break;
			case 'r': i++; kernel_parm->coef_const=atof(argv[i]); break;
			case 'u': i++; strcpy(kernel_parm->custom,argv[i]); break;
			case 'l': i++; strcpy(learn_parm->predfile,argv[i]); break;
			case 'a': i++; strcpy(learn_parm->alphafile,argv[i]); break;
			case 'y': i++; strcpy(restartfile,argv[i]); break;
			case '-':
				switch (argv[i][2]) {
					case 'm': i++; strcpy(modelfile, argv[i]); break;
					case 'c': i++; strcpy(confmatfile, argv[i]); break;
				}
				break;
			default: printf("\nUnrecognized option %s!\n\n",argv[i]);
				print_help();
				exit(0);
		}
	}
	if(i>=argc) {
		printf("\nNot enough input parameters!\n\n");
		wait_any_key();
		print_help();
		exit(0);
	}
	strcpy (docfile, argv[i]);
	if(learn_parm->svm_iter_to_shrink == -9999) {
		if(kernel_parm->kernel_type == LINEAR)
			learn_parm->svm_iter_to_shrink=2;
		else
			learn_parm->svm_iter_to_shrink=100;
	}
	if(strcmp(type,"c")==0) {
		learn_parm->type=CLASSIFICATION;
	}
	else if(strcmp(type,"r")==0) {
		learn_parm->type=REGRESSION;
	}
	else if(strcmp(type,"p")==0) {
		learn_parm->type=RANKING;
	}
	else if(strcmp(type,"o")==0) {
		learn_parm->type=OPTIMIZATION;
	}
	else if(strcmp(type,"s")==0) {
		learn_parm->type=OPTIMIZATION;
		learn_parm->sharedslack=1;
	}
	else {
		printf("\nUnknown type '%s': Valid types are 'c' (classification), 'r' regession, and 'p' preference ranking.\n",type);
		wait_any_key();
		print_help();
		exit(0);
	}
	if (!check_learning_parms(learn_parm, kernel_parm)) {
		wait_any_key();
		print_help();
		exit(0);
	}
}

void wait_any_key()
{
	printf("\n(more)\n");
	(void)getc(stdin);
}

void print_help()
{
	printf("\nSVM-light %s: Support Vector Machine, learning module     %s\n",VERSION,VERSION_DATE);
	copyright_notice();
	printf("   usage: learn_confusion_matrix [options] example_file model_file\n\n");
	printf("Arguments:\n");
	printf("         example_file-> file with training data\n");
	printf("         model_file  -> file to store learned decision rule in\n");

	printf("General options:\n");
	printf("         -?          -> this help\n");
	printf("         -v [0..3]   -> verbosity level (default 1)\n");
	printf("Learning options:\n");
	printf("         -z {c,r,p}  -> select between classification (c), regression (r),\n");
	printf("                        and preference ranking (p) (default classification)\n");
	printf("         -c float    -> C: trade-off between training error\n");
	printf("                        and margin (default [avg. x*x]^-1)\n");
	printf("         -w [0..]    -> epsilon width of tube for regression\n");
	printf("                        (default 0.1)\n");
	printf("         -j float    -> Cost: cost-factor, by which training errors on\n");
	printf("                        positive examples outweight errors on negative\n");
	printf("                        examples (default 1) (see [4])\n");
	printf("         -b [0,1]    -> use biased hyperplane (i.e. x*w+b>0) instead\n");
	printf("                        of unbiased hyperplane (i.e. x*w>0) (default 1)\n");
	printf("         -i [0,1]    -> remove inconsistent training examples\n");
	printf("                        and retrain (default 0)\n");
	printf("Performance estimation options:\n");
	printf("         -x [0,1]    -> compute leave-one-out estimates (default 0)\n");
	printf("                        (see [5])\n");
	printf("         -o ]0..2]   -> value of rho for XiAlpha-estimator and for pruning\n");
	printf("                        leave-one-out computation (default 1.0) (see [2])\n");
	printf("         -k [0..100] -> search depth for extended XiAlpha-estimator \n");
	printf("                        (default 0)\n");
	printf("Transduction options (see [3]):\n");
	printf("         -p [0..1]   -> fraction of unlabeled examples to be classified\n");
	printf("                        into the positive class (default is the ratio of\n");
	printf("                        positive and negative examples in the training data)\n");
	printf("Kernel options:\n");
	printf("         -t int      -> type of kernel function:\n");
	printf("                        0: linear (default)\n");
	printf("                        1: polynomial (s a*b+c)^d\n");
	printf("                        2: radial basis function exp(-gamma ||a-b||^2)\n");
	printf("                        3: sigmoid tanh(s a*b + c)\n");
	printf("                        4: user defined kernel from kernel.h\n");
	printf("         -d int      -> parameter d in polynomial kernel\n");
	printf("         -g float    -> parameter gamma in rbf kernel\n");
	printf("         -s float    -> parameter s in sigmoid/poly kernel\n");
	printf("         -r float    -> parameter c in sigmoid/poly kernel\n");
	printf("         -u string   -> parameter of user defined kernel\n");
	printf("Optimization options (see [1]):\n");
	printf("         -q [2..]    -> maximum size of QP-subproblems (default 10)\n");
	printf("         -n [2..q]   -> number of new variables entering the working set\n");
	printf("                        in each iteration (default n = q). Set n < q to \n");
	printf("                        prevent zig-zagging.\n");
	printf("         -m [5..]    -> size of cache for kernel evaluations in MB (default 40)\n");
	printf("                        The larger the faster...\n");
	printf("         -e float    -> eps: Allow that error for termination criterion\n");
	printf("                        [y [w*x+b] - 1] >= eps (default 0.001)\n");
	printf("         -y [0,1]    -> restart the optimization from alpha values in file\n");
	printf("                        specified by -a option. (default 0)\n");
	printf("         -h [5..]    -> number of iterations a variable needs to be\n");
	printf("                        optimal before considered for shrinking (default 100)\n");
	printf("         -f [0,1]    -> do final optimality check for variables removed\n");
	printf("                        by shrinking. Although this test is usually \n");
	printf("                        positive, there is no guarantee that the optimum\n");
	printf("                        was found if the test is omitted. (default 1)\n");
	printf("         -y string   -> if option is given, reads alphas from file with given\n");
	printf("                        and uses them as starting point. (default 'disabled')\n");
	printf("         -# int      -> terminate optimization, if no progress after this\n");
	printf("                        number of iterations. (default 100000)\n");
	printf("Output options:\n");
	printf("         -l string   -> file to write predicted labels of unlabeled\n");
	printf("                        examples into after transductive learning\n");
	printf("         -a string   -> write all alphas to this file after learning\n");
	printf("                        (in the same order as in the training set)\n");
	wait_any_key();
	printf("\nMore details in:\n");
	printf("[1] T. Joachims, Making Large-Scale SVM Learning Practical. Advances in\n");
	printf("    Kernel Methods - Support Vector Learning, B. Schˆlkopf and C. Burges and\n");
	printf("    A. Smola (ed.), MIT Press, 1999.\n");
	printf("[2] T. Joachims, Estimating the Generalization performance of an SVM\n");
	printf("    Efficiently. International Conference on Machine Learning (ICML), 2000.\n");
	printf("[3] T. Joachims, Transductive Inference for Text Classification using Support\n");
	printf("    Vector Machines. International Conference on Machine Learning (ICML),\n");
	printf("    1999.\n");
	printf("[4] K. Morik, P. Brockhausen, and T. Joachims, Combining statistical learning\n");
	printf("    with a knowledge-based approach - A case study in intensive care  \n");
	printf("    monitoring. International Conference on Machine Learning (ICML), 1999.\n");
	printf("[5] T. Joachims, Learning to Classify Text Using Support Vector\n");
	printf("    Machines: Methods, Theory, and Algorithms. Dissertation, Kluwer,\n");
	printf("    2002.\n\n");
}
