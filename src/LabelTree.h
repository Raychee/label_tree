# ifndef _LABELTREE_H
# define _LABELTREE_H

# include "SGD.h"


typedef double COMP_T;
// type of the value to be computed (parameters, training samples, etc)
typedef unsigned long SUPV_T;
// type of the supervising information (classes, labels, etc)
typedef unsigned long N_DAT_T;
// type of the number of the data set
typedef unsigned int DAT_DIM_T;
// type of the dimension of the data


class LabelTree : public SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T> {
// Label Tree with an SGD optimizer for joint convex optimization

public:
	class LabelTreeNode;

	LabelTree(DAT_DIM_T 	 _d,
			  SUPV_T 		 _s_labelset,
			  unsigned int   _nary        	  = 2,
			  char           _verbosity   	  = 1,
        	  COMP_T         _eta0        	  = 0,
        	  unsigned int   _n_epoch     	  = 5,
        	  COMP_T         _eta0_1st_try	  = 0.5,
        	  COMP_T         _eta0_try_factor = 2);
	LabelTree(LabelTree& tree);
	~LabelTree();
	
	LabelTreeNode* root() const { return root_; }

	virtual void   train_one(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y);
	virtual SUPV_T test_one(COMP_T* dat_i, DAT_DIM_T d) const;
	virtual COMP_T compute_obj(COMP_T* dat, DAT_DIM_T d, N_DAT_T n,
                               SUPV_T* y) const;
	virtual std::ostream& output_stream(std::ostream& out) const;
private:
	COMP_T         lambda;		// regularizing term's coefficient
	LabelTreeNode* root_;		// root of the tree

	virtual LabelTree* get_temp_dup() {
		return new LabelTree(*this);
	}
	virtual COMP_T compute_learning_rate() {
		return eta0 / (1 + lambda * eta0 * t);
	}
};

class LabelTree::LabelTreeNode {
// An n-nary Label Tree's Node
// Each node contains a learning model "w", its label set "labelset" and has
// at most "n_nary" children. The labels are integers (1, 2, ...), implying the 
// classes of the data it contains. Label "0" refers to data with no supervising
// information (e.g. testing data)
public:
	LabelTreeNode(DAT_DIM_T _d, unsigned int _nary);
	LabelTreeNode(DAT_DIM_T _d, unsigned int _nary,
				  SUPV_T* _labelset, SUPV_T _s_labelset);
	LabelTreeNode(DAT_DIM_T _d, unsigned int _nary, SUPV_T _s_labelset);
	LabelTreeNode(LabelTreeNode& node);
	~LabelTreeNode();
	unsigned int    nary() const { return n_nary; }
	SUPV_T*         labels() const { return labelset; }
	LabelTreeNode&  labels(SUPV_T* _labelset, SUPV_T _s_labelset);
	SUPV_T          num_of_labels() const { return s_labelset; }
	LabelTreeNode*  parent() const { return parent_; }
	LabelTreeNode** children() const { return child; }
	unsigned int    num_of_children() const { return n_child; }
	bool            is_full_of_children() const { return n_child >= n_nary; }
	LabelTreeNode&  attach_child(LabelTreeNode* ch);
	// attach one node "ch" to the current node as one of its child
	LabelTreeNode&  attach_child(SUPV_T* _labelset, SUPV_T _s_labelset);
	// attach one node which has a label set "labelset" of size "s_labelset" to
	// the current node as one of its child
	LabelTreeNode&  attach_children(LabelTreeNode** _child,
									unsigned int _n_child);
	// attach "_n_child" nodes in the list "_child" to the current node as its 
	// children

private:
	unsigned int    n_nary;			// number of branches of the tree
	COMP_T*         w;				// learning model (e.g. a linear classifier)
	COMP_T 			b;				// bias term in the learning model
	const DAT_DIM_T d;				// dimension of the model
	SUPV_T          s_labelset;		// size of the label set
	SUPV_T*         labelset;		// label set (array of labels)
	unsigned int    n_child;		// number of children of this node
	LabelTreeNode** child;			// list of children
	LabelTreeNode*  parent_;		// pointer to the parent
};

# endif
