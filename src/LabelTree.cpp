# include <cstring>
# include <iostream>
# include <iomanip>
# include <queue>
# include "LabelTree.h"


/*******************  LabelTree::LabelTreeNode definitions  *******************/
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
	// std::cout << "CREATED:\n";
	// output_stream(std::cout);
	// std::cout << "\n";
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
	// std::cout << "CREATED(default):\n";
	// output_stream(std::cout);
	// std::cout << "\n";
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
	// std::cout << "DELETED:\n";
	// output_stream(std::cout);
	// std::cout << "\n";
	delete[] w;
	delete[] labelset;
	delete[] child;
}

LabelTree::LabelTreeNode& LabelTree::
LabelTreeNode::labels(SUPV_T* _labelset, SUPV_T _s_labelset) {
	if (labelset && s_labelset) delete[] labelset;
	labelset = new SUPV_T[_s_labelset];
	s_labelset = _s_labelset;
	std::memcpy(labelset, _labelset, sizeof(SUPV_T) * s_labelset);
}

bool LabelTree::
LabelTreeNode::has_label(SUPV_T _label) const {
	if (labelset[0] == _label) return true;
	SUPV_T begin = 0;
	SUPV_T end   = s_labelset;
	SUPV_T mid   = s_labelset / 2;
	SUPV_T label = labelset[mid];
	while (_label != label) {
		if (_label > label) begin = mid;
		else end = mid;
		if (end - begin <= 1) {
			return false;
		}
		mid = (begin + end) / 2;
		label = labelset[mid];
	}
	return true;
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

LabelTree::LabelTreeNode&  LabelTree::
LabelTreeNode::output_stream(std::ostream& out) {
	out << "LabelTreeNode(" << this << ", parent = "
		<< parent_ << ")\n    labels   = ";
	for (SUPV_T i = 0; i < s_labelset; ++i) {
		out << " " << std::setw(3) << std::setiosflags(std::ios_base::left)
			<< labelset[i];
	}
	out << "\n    children = ";
	if (n_child) {
		for (unsigned int i = 0; i < n_child; i++) {
			out << " " << child[i];
		}
	}
	else {
		out << "NULL";
	}
	out << "\n}";
	return *this;
}

COMP_T LabelTree::
LabelTreeNode::compute_score(COMP_T* dat_i) {
	COMP_T score = 0;
	for (DAT_DIM_T i = 0; i < d; ++i) {
		score += dat_i[i] * w[i];
	}
	score += b;
	return score;
}

LabelTree::LabelTreeNode&  LabelTree::
LabelTreeNode::update_no_loss(COMP_T eta, COMP_T lambda) {
	COMP_T factor = 1 - eta * lambda;
	for (DAT_DIM_T i = 0; i < d; ++i) {
		w[i] *= factor;
	}
	return *this;
}

LabelTree::LabelTreeNode&  LabelTree::
LabelTreeNode::update_reduce_score(COMP_T eta, COMP_T lambda,
								   COMP_T* dat_i, N_DAT_T n) {
	COMP_T term_1 = 1 - eta * lambda;
	COMP_T term_2 = eta / n;
	for (DAT_DIM_T i = 0; i < d; ++i) {
		w[i] = w[i] * term_1 - dat_i[i] * term_2;
	}
	b -= term_2;
	return *this;
}


LabelTree::LabelTreeNode&  LabelTree::
LabelTreeNode::update_incre_score(COMP_T eta, COMP_T lambda,
								  COMP_T* dat_i, N_DAT_T n) {
	COMP_T term_1 = 1 - eta * lambda;
	COMP_T term_2 = eta / n;
	for (DAT_DIM_T i = 0; i < d; ++i) {
		w[i] = w[i] * term_1 + dat_i[i] * term_2;
	}
	b += term_2;
	return *this;
}


/*********************  LabelTree::iterator definitions  **********************/
LabelTree::iterator::iterator(LabelTreeNode* node)
		  :pointer(node) {
	if (pointer) breadth_first_traverse.push(pointer);
}

LabelTree::iterator& LabelTree::iterator::operator++() {
	unsigned int n_child = pointer->num_of_children();
	if (n_child) {
		LabelTreeNode** child = pointer->children();
		for (unsigned int i = 0; i < n_child; ++i) {
			breadth_first_traverse.push(child[i]);
		}
	}
	breadth_first_traverse.pop();
	if (breadth_first_traverse.empty()) pointer = NULL;
	else pointer = breadth_first_traverse.front();
	return *this;
}

LabelTree::iterator LabelTree::iterator::operator++(int) {
	iterator temp(*this);
	unsigned int n_child = pointer->num_of_children();
	if (n_child) {
		LabelTreeNode** child = pointer->children();
		for (unsigned int i = 0; i < n_child; ++i) {
			breadth_first_traverse.push(child[i]);
		}
	}
	breadth_first_traverse.pop();
	if (breadth_first_traverse.empty()) pointer = NULL;
	else pointer = breadth_first_traverse.front();
	return temp;
}


/**************************  LabelTree definitions  ***************************/
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
	for (iterator i = begin(); i != end(); ++i) {
		delete &i;
	}
}

LabelTree& LabelTree::train_one(COMP_T* dat, N_DAT_T i,
              					DAT_DIM_T d, N_DAT_T n, SUPV_T y) {
	COMP_T* 	   dat_i      = dat + d * i;
	LabelTreeNode* start_node = root_;
	unsigned int   n_child    = start_node->num_of_children();
	COMP_T 		   score      = new COMP_T[root_->nary()];
	while (n_child) {
		LabelTreeNode** child = start_node->children();
		std::memset(score, 0, sizeof(COMP_T) * n_child);
		for (unsigned int i = 0; i < n_child; ++i) {
			score[i] = child[i]->compute_score(dat_i);
		}
		unsigned int score_i = max(score, n_child);
		if (child[score_i]->has_label(y)) {
			for (unsigned int i = 0; i < n_child; ++i) {
				child[i]->update_no_loss(eta, lambda);
			}
			start_node = child[score_i];
		}
		else {
			for (unsigned int i = 0; i < n_child; ++i) {
				if (child[i]->has_label(y)) {
					child[i]->update_incre_score(eta, lambda, dat_i, n);
					start_node = child[i];
				}
				else if (i == score_i) {
					child[score_i]->update_reduce_score(eta, lambda, dat_i, n);					
				}
				else {
					child[i]->update_no_loss;
				}
			}
		}
		n_child = start_node->num_of_children();
	}
}

SUPV_T LabelTree::test_one(COMP_T* dat_i, DAT_DIM_T d) const {
	LabelTreeNode* node    = root_;
	unsigned int   n_child = start_node->num_of_children();
	COMP_T 		   score   = new COMP_T[root_->nary()];
	while (n_child) {
		LabelTreeNode** child = node->children();
		std::memset(score, 0, sizeof(COMP_T) * n_child);
		for (unsigned int i = 0; i < n_child; ++i) {
			score[i] = child[i]->compute_score(dat_i);
		}
		unsigned int score_i = max(score, n_child);
		node = child[score_i];
		n_child = node->num_of_children();
	}
	SUPV_T* labelset = node->labels();
	return labelset[0];
}

COMP_T LabelTree::compute_obj(COMP_T* dat, DAT_DIM_T d, N_DAT_T n,
							  SUPV_T* y) const {

}

LabelTree& LabelTree::output_stream(std::ostream& out) {
	out << "Tree Nodes are as follows (start from root):";
	for (iterator i = begin(); i != end(); ++i) {
		out << "\n";
		i->output_stream(out);
	}
	return *this;
}

unsigned int max(COMP_T* array, unsigned int length) {
	COMP_T       cur_max   = array[0];
	unsigned int cur_max_i = 0;
	for (unsigned int i = 1; i < length; ++i) {
		if (array[i] > cur_max) {
			cur_max   = array[i];
			cur_max_i = i;
		}
	}
	return cur_max_i;
}
