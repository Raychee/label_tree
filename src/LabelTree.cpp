# include <cstring>
# include <iostream>
# include <iomanip>
# include <queue>
# include "LabelTree.h"


/******** LabelTreeNode functions ********/
/**** Constructor ****/
LabelTree::
LabelTreeNode::LabelTreeNode(DAT_DIM_T _d, unsigned int _nary)
			  :n_nary(_nary),
			   d(_d),
			   s_labelset(0),
			   labelset(NULL),
			   n_child(0),
			   parent_(NULL) {
	w     = new COMP_T[d];
	child = new LabelTreeNode*[n_nary];
}
LabelTree::
LabelTreeNode::LabelTreeNode(DAT_DIM_T 	  _d,
							 unsigned int _nary,
							 SUPV_T*      _labelset,
							 SUPV_T       _s_labelset)
			  :n_nary(_nary),
			   d(_d),
			   s_labelset(_s_labelset),
			   n_child(0),
			   parent_(NULL) {
	w        = new COMP_T[d];
	labelset = new SUPV_T[s_labelset];
	child    = new LabelTreeNode*[n_nary];
	std::memcpy(labelset, _labelset, sizeof(SUPV_T) * s_labelset);
}

LabelTree::
LabelTreeNode::LabelTreeNode(DAT_DIM_T _d,
							 unsigned int _nary,
							 SUPV_T _s_labelset)
			  :n_nary(_nary),
			   d(_d),
			   s_labelset(_s_labelset),
			   n_child(0),
			   parent_(NULL) {
	w        = new COMP_T[d];
	labelset = new SUPV_T[s_labelset];
	child    = new LabelTreeNode*[n_nary];
	for (SUPV_T i = 1; i <= s_labelset; ++i) {
		labelset[i] = i;
	}
}

LabelTree::
LabelTreeNode::LabelTreeNode(LabelTree::LabelTreeNode& node)
			  :n_nary(node.n_nary),
			   b(node.b),
			   d(node.d),
			   s_labelset(node.s_labelset),
			   n_child(node.n_child),
			   parent_(NULL) {
	w        = new COMP_T[d];
	labelset = new SUPV_T[s_labelset];
	child    = new LabelTreeNode*[n_nary];
	std::memcpy(w, node.w, sizeof(COMP_T) * d);
	std::memcpy(labelset, node.labelset, sizeof(SUPV_T) * s_labelset);
}

/**** Destructor ****/
LabelTree::
LabelTreeNode::~LabelTreeNode() {
	delete[] w;
	delete[] labelset;
	delete[] child;
}

LabelTree::LabelTreeNode& LabelTree::
LabelTreeNode::attach_child(LabelTreeNode* ch) {
	if (n_child >= n_nary) {
		std::cerr << "Error while attaching a child: "
				  << "LabelTreeNode is already full." << std::endl;
		return *this;
	}
	ch->parent_ = this;
	child[n_child++] = ch;
	return *this;
}

LabelTree::LabelTreeNode& LabelTree::
LabelTreeNode::attach_child(SUPV_T* _labelset, SUPV_T _s_labelset) {
	if (n_child >= n_nary) {
		std::cerr << "Error while attaching a child: "
				  << "LabelTreeNode is already full." << std::endl;
		return *this;
	}
	LabelTreeNode* ch = new LabelTreeNode(d, n_nary, _labelset, _s_labelset);
	ch->parent_ = this;
	child[n_child++] = ch;
	return *this;
}

LabelTree::LabelTreeNode& LabelTree::
LabelTreeNode::attach_children(LabelTreeNode** _child,
							   unsigned int    _n_child) {
	if (_n_child >= n_nary) {
		std::cerr << "Error while attaching children: Too many children. ("
			 << n_nary << "children at most while trying to attach "
			 << _n_child << ")" << std::endl;
		return *this;
	}
	for (unsigned int i = 0; i < _n_child; i++) {
		_child[i]->parent_ = this;
		child[i] = _child[i];
	}
	n_child = _n_child;
	return *this;
}


/******** LabelTree functions ********/
/**** Constructor ****/
LabelTree::LabelTree(DAT_DIM_T 	  _d,
			  		 SUPV_T       _s_labelset,
					 unsigned int _nary,
			  		 char         _verbosity,
        	  		 COMP_T       _eta0,
        	  		 unsigned int _n_epoch,
        	  		 COMP_T       _eta0_1st_try,
        	  		 COMP_T       _eta0_try_factor)
          :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(_verbosity,
          										   _eta0,
          										   _n_epoch,
          										   _eta0_1st_try,
          										   _eta0_try_factor) {
    root_ = new LabelTreeNode(_d, _nary, _s_labelset);
}

LabelTree::LabelTree(LabelTree& tree)
		  :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(tree),
		   lambda(tree.lambda) {
	root_ = new LabelTreeNode(*tree.root_);
	std::queue<LabelTreeNode*> breadth_first_traverse;
	std::queue<LabelTreeNode*> breadth_first_traverse_dup;
	LabelTreeNode* cur, * cur_dup;
	LabelTreeNode** cur_ch;
	unsigned int cur_n_ch;
	breadth_first_traverse.push(tree.root_);
	breadth_first_traverse_dup.push(root_);
	while (!breadth_first_traverse.empty()) {
		cur = breadth_first_traverse.front();
		cur_n_ch = cur->num_of_children();
		if (cur_n_ch) {
			cur_dup = breadth_first_traverse_dup.front();
			cur_ch = cur->children();
			for (unsigned int i = 0; i < cur_n_ch; i++) {
				LabelTreeNode* dup = new LabelTreeNode(*cur_ch[i]);
				cur_dup->attach_child(dup);
				breadth_first_traverse.push(cur_ch[i]);
				breadth_first_traverse_dup.push(dup);
			}
		}
		breadth_first_traverse.pop();
		breadth_first_traverse_dup.pop();
	}
}

/**** Destructor ****/
LabelTree::~LabelTree() {
	std::queue<LabelTreeNode*> breadth_first_traverse;
	LabelTreeNode* cur;
	LabelTreeNode** cur_ch;
	unsigned int cur_n_ch;
	breadth_first_traverse.push(root_);
	while (!breadth_first_traverse.empty()) {
		cur = breadth_first_traverse.front();
		cur_n_ch = cur->num_of_children();
		if (cur_n_ch) {
			cur_ch = cur->children();
			for (unsigned int i = 0; i < cur_n_ch; i++) {
				breadth_first_traverse.push(cur_ch[i]);
			}
		}
		breadth_first_traverse.pop();
		delete cur;
	}
}

void LabelTree::train_one(COMP_T* dat_i, DAT_DIM_T d, SUPV_T y) {

}

SUPV_T LabelTree::test_one(COMP_T* dat_i, DAT_DIM_T d) const {

}

COMP_T LabelTree::compute_obj(COMP_T* dat, DAT_DIM_T d, N_DAT_T n,
							  SUPV_T* y) const {

}

std::ostream& LabelTree::output_stream(std::ostream& out) const {
	std::queue<LabelTreeNode*> breadth_first_traverse;
	LabelTreeNode* cur;
	LabelTreeNode** cur_ch;
	SUPV_T* labels, n_labels;
	unsigned int cur_n_ch;
	out << "Tree Nodes are as follows (start from root):";
	breadth_first_traverse.push(root_);
	while (!breadth_first_traverse.empty()) {
		cur = breadth_first_traverse.front();
		labels = cur->labels();
		n_labels = cur->num_of_labels();
		out << "\nLabelTreeNode(" << cur << ", parent = "
			<< cur->parent() << ")\n\tlabels =\t";
		for (SUPV_T i = 0; i < n_labels; i++) {
			out << " " << std::setw(3) << std::setiosflags(std::ios_base::left)
				<< labels[i];
		}
		out << "\n\tchildren =\t";
		cur_n_ch = cur->num_of_children();
		if (cur_n_ch) {
			cur_ch = cur->children();
			for (unsigned int i = 0; i < cur_n_ch; i++) {
				out << " " << cur_ch[i];
				breadth_first_traverse.push(cur_ch[i]);
			}
		}
		else {
			out << "NULL";
		}
		out << "\n}";
		breadth_first_traverse.pop();
	}
	return out;
}
