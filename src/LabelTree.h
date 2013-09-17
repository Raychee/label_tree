# ifndef _LABELTREE_H
# define _LABELTREE_H

# include <queue>
# include "SGD.h"


typedef double        COMP_T;
// type of the value to be computed (parameters, training samples, etc)
typedef unsigned long SUPV_T;
// type of the supervising information (classes, labels, etc)
typedef unsigned long N_DAT_T;
// type of the number of the data set
typedef unsigned int  DAT_DIM_T;
// type of the dimension of the data


class LabelTree : public SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T> {
// Label Tree with an SGD optimizer for joint convex optimization: 
// n samples, m tree nodes
// Parameters: w_j(j=1:m), b_j(j=1:m), lambda, eta(learning rate)
// obj = 0.5 * lambda * sum(j=1:m)(w_j' * w_j) + sum(i=1:n)(loss_i)
// loss_i = max{0, (w_j - w_k)' * x_i + b_j - b_k (arbitary j, k s.t. 
// 			node j & k are siblings && y_i ∈ labelset_k && y_i ∉ labelset_j) }
// Update rules:
// Given a training sample x_i, starting from the root, for every node j s.t.
// y_i ∈ labelset_j, do the following:
// 1. Obtain the children's list "child" of the node j. Assume node j has
// 	  "n_child" children, then the list "child" is of length "n_child".
// 2. For every node k in "child", compute its score:
//    score_k = w_k' * x_i + b_k  (k=1:n_child)
// 3. Find the node s with the largest score "score_s".
// 4. If y_i ∈ labelset_s, then EVERY node k in "child" do the following:
//    w_k = (1 - eta * lambda) * w_k, b_k = b_k.
// 5. If y_i ∉ labelset_s, then for node s,
//    w_s = (1 - eta * lambda) * w_s - (1/n) * eta * x_i,
//    b_s = b_s - (1/n) * eta.
// 	  For the node r which y_i ∈ labelset_r,
// 	  w_r = (1 - eta * lambda) * w_s + (1/n) * eta * x_i,
// 	  b_s = b_s + (1/n) * eta.
// 6. Take node r as the starting node, repeat 1 ~ 6 until r is a leaf.

public:
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

	virtual LabelTree& train_one(COMP_T* dat, N_DAT_T i,
              					 DAT_DIM_T d, N_DAT_T n, SUPV_T y);
	virtual SUPV_T     test_one(COMP_T* dat_i, DAT_DIM_T d) const;
	virtual COMP_T     compute_obj(COMP_T* dat, DAT_DIM_T d, N_DAT_T n,
                               	   SUPV_T* y) const;
	virtual LabelTree& output_stream(std::ostream& out);

	class LabelTreeNode;
	LabelTreeNode* root() const { return root_; }

	class iterator {
	public:
		// iterator() {};
		iterator(LabelTreeNode* node);
		~iterator() {};
		iterator&      operator++();
		iterator       operator++(int);
		LabelTreeNode& operator*()  const { return *pointer; }
		LabelTreeNode* operator&()  const { return pointer; }
		LabelTreeNode* operator->() const { return pointer; }
		bool           operator==(const iterator& some) const {
			return pointer == some.pointer;
		}
		bool           operator!=(const iterator& some) const {
			return pointer != some.pointer;
		}
	private:
		LabelTreeNode* pointer;
		std::queue<LabelTreeNode*> breadth_first_traverse;
	};
	iterator begin() const { return iterator(root_); }
	iterator end()   const { return iterator(NULL); }

private:
	COMP_T         lambda;		// regularizing term's coefficient
	LabelTreeNode* root_;		// root of the tree

	virtual LabelTree* get_temp_dup() {
		return new LabelTree(*this);
	}
	virtual COMP_T     compute_learning_rate() {
		return eta0 / (1 + lambda * eta0 * t);
	}
	unsigned int max(COMP_T* array, unsigned int length);
	// return the index of the largest element in "array"
};

class LabelTree::LabelTreeNode {
// An n-nary Label Tree's Node
// Each node contains a learning model "w", its label set "labelset" and has
// at most "n_nary" children. The labels are integers (1, 2, ...), implying the 
// classes of the data it contains. Label "0" refers to data with no supervising
// information (e.g. testing data). Labels in "labelset" are sorted in ASCENDING
// order.
public:
	LabelTreeNode(DAT_DIM_T _d, unsigned int _nary);
	LabelTreeNode(DAT_DIM_T _d, unsigned int _nary,
				  SUPV_T* _labelset, SUPV_T _s_labelset);
	LabelTreeNode(DAT_DIM_T _d, unsigned int _nary, SUPV_T _s_labelset);
	LabelTreeNode(LabelTreeNode& node);
	~LabelTreeNode();
	const unsigned int nary()                const { return n_nary; }
	SUPV_T*            labels()              const { return labelset; }
	LabelTreeNode&     labels(SUPV_T* _labelset, SUPV_T _s_labelset);
	bool			   has_label(SUPV_T _label) const;
	SUPV_T             num_of_labels()       const { return s_labelset; }
	LabelTreeNode&     parent()              const { return *parent_; }
	LabelTreeNode&	   child(unsigned int i) const { return *child[i]; }
	LabelTreeNode**    children()            const { return child; }
	unsigned int       num_of_children()     const { return n_child; }
	bool               is_full_of_children() const { return n_child >= n_nary; }
	LabelTreeNode&     attach_child(LabelTreeNode* ch);
	// attach one node "ch" to the current node as one of its child
	LabelTreeNode&     attach_child(SUPV_T* _labelset, SUPV_T _s_labelset);
	// attach one node which has a label set "labelset" of size "s_labelset" to
	// the current node as one of its child
	LabelTreeNode&     attach_children(LabelTreeNode** _child,
									unsigned int _n_child);
	// attach "_n_child" nodes in the list "_child" to the current node as its 
	// children
	LabelTreeNode&     output_stream(std::ostream& out);

	COMP_T             compute_score(COMP_T* dat_i);
	// compute w' * x_i + b
	LabelTreeNode&     update_no_loss(COMP_T eta, COMP_T lambda);
	// update w & b following the LabelTree's update rule 4
	LabelTreeNode&     update_reduce_score(COMP_T eta, COMP_T lambda,
										COMP_T* dat_i, N_DAT_T n);
	// update w & b following the LabelTree's update rule 5
	LabelTreeNode&     update_incre_score(COMP_T eta, COMP_T lambda,
										COMP_T* dat_i, N_DAT_T n);
	// update w & b following the LabelTree's update rule 5

private:
	const unsigned int n_nary;		// number of branches of the tree
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
