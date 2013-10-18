# include <cstdlib>
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
			   b(0),
			   d(_d),
			   s_labelset(0),
			   labelset(NULL),
			   n_child(0),
			   child_(NULL),
			   parent_(NULL) {
	w = new COMP_T[d];
	// for (DAT_DIM_T i = 0; i < d; ++i) {
	// 	w[i] = (COMP_T)std::rand() / RAND_MAX;
	// }
	// b = (COMP_T)std::rand() / RAND_MAX;
	std::memset(w, 0, sizeof(COMP_T) * d);
}
LabelTree::
LabelTreeNode::LabelTreeNode(DAT_DIM_T 	  _d,
							 unsigned int _nary,
							 SUPV_T*      _labelset,
							 SUPV_T       _s_labelset,
							 bool 		  use_memcpy)
			  :n_nary(_nary),
			   // b(0),
			   d(_d),
			   s_labelset(_s_labelset),
			   n_child(0),
			   child_(NULL),
			   parent_(NULL) {
	w = new COMP_T[d];
	for (DAT_DIM_T i = 0; i < d; ++i) {
		w[i] = (COMP_T)std::rand() / RAND_MAX;
	}
	b = (COMP_T)std::rand() / RAND_MAX;
	// std::memset(w, 0, sizeof(COMP_T) * d);
	if (use_memcpy) {
		labelset = new SUPV_T[s_labelset];
		std::memcpy(labelset, _labelset, sizeof(SUPV_T) * s_labelset);
	}
	else {
		labelset = _labelset;
	}
	// std::cout << "CREATED:\n";
	// ostream_this(std::cout);
	// std::cout << "\n";
}

LabelTree::
LabelTreeNode::LabelTreeNode(DAT_DIM_T _d,
							 unsigned int _nary,
							 SUPV_T _s_labelset)
			  :n_nary(_nary),
			   // b(0),
			   d(_d),
			   s_labelset(_s_labelset),
			   n_child(0),
			   child_(NULL),
			   parent_(NULL) {
	w        = new COMP_T[d];
	labelset = new SUPV_T[s_labelset];
	for (DAT_DIM_T i = 0; i < d; ++i) {
		w[i] = (COMP_T)std::rand() / RAND_MAX;
	}
	b = (COMP_T)std::rand() / RAND_MAX;
	// std::memset(w, 0, sizeof(COMP_T) * d);	
	for (SUPV_T i = 0; i < s_labelset; ++i) {
		labelset[i] = i + 1;
	}
	// std::cout << "CREATED(default):\n";
	// ostream_this(std::cout);
	// std::cout << "\n";
}

LabelTree::
LabelTreeNode::LabelTreeNode(LabelTree::LabelTreeNode& node)
			  :n_nary(node.n_nary),
			   b(node.b),
			   d(node.d),
			   s_labelset(node.s_labelset),
			   n_child(0),
			   child_(NULL),
			   parent_(NULL) {
	w        = new COMP_T[d];
	labelset = new SUPV_T[s_labelset];
	std::memcpy(w, node.w, sizeof(COMP_T) * d);
	std::memcpy(labelset, node.labelset, sizeof(SUPV_T) * s_labelset);
}

/**** Destructor ****/
LabelTree::
LabelTreeNode::~LabelTreeNode() {
	// std::cout << "DELETED:\n";
	// ostream_this(std::cout);
	// std::cout << "\n";
	delete[] w;
	delete[] labelset;
	delete[] child_;
}

LabelTree::LabelTreeNode& LabelTree::
LabelTreeNode::labels(SUPV_T* _labelset, SUPV_T _s_labelset,
					  bool use_memcpy) {
	if (labelset && s_labelset) delete[] labelset;
	if (use_memcpy) {
		labelset = new SUPV_T[_s_labelset];
		s_labelset = _s_labelset;
		std::memcpy(labelset, _labelset, sizeof(SUPV_T) * s_labelset);
	}
	else {
		labelset   = _labelset;
		s_labelset = _s_labelset;
	}
	return *this;
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
		mid   = (begin + end) / 2;
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
	if (!n_child || !child_) {
		child_ = new LabelTreeNode*[n_nary];
	}
	ch->parent_       = this;
	child_[n_child++] = ch;
	return *this;
}

LabelTree::LabelTreeNode& LabelTree::
LabelTreeNode::attach_child(SUPV_T* _labelset, SUPV_T _s_labelset,
							bool use_memcpy) {
	if (n_child >= n_nary) {
		std::cerr << "Error while attaching a child: "
				  << "LabelTreeNode is already full." << std::endl;
		return *this;
	}
	LabelTreeNode* ch = new LabelTreeNode(d, n_nary, _labelset, _s_labelset,
										  use_memcpy);
	if (!n_child || !child_) {
		child_ = new LabelTreeNode*[n_nary];
	}
	ch->parent_       = this;
	child_[n_child++] = ch;
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
	if (!n_child || !child_) {
		child_ = new LabelTreeNode*[n_nary];
	}
	for (unsigned int i = 0; i < _n_child; i++) {
		_child[i]->parent_ = this;
		child_[i]          = _child[i];
	}
	n_child = _n_child;
	return *this;
}

LabelTree::LabelTreeNode&  LabelTree::
LabelTreeNode::ostream_this(std::ostream& out) {
	out.setf(std::ios_base::left, std::ios_base::adjustfield);
	out << "LabelTreeNode(" << this << ", parent = "
		<< parent_ << ")\n    labels   = ";
	for (SUPV_T i = 0; i < s_labelset; ++i) {
		out << " " << std::setw(3) << labelset[i];
	}
	out << " {\n    w        =";
	for (DAT_DIM_T i = 0; i < d; ++i) {
		out << " " << std::setw(14) << w[i];
	}
	out << "\n    b        = " << std::setw(14) << b;
	
	out << "\n    children =";
	if (n_child) {
		for (unsigned int i = 0; i < n_child; i++) {
			out << " " << child_[i];
		}
	}
	else {
		out << " NULL";
	}
	out << "\n}";
	return *this;
}

COMP_T LabelTree::
LabelTreeNode::compute_score(COMP_T* dat_i) const {
	COMP_T score = 0;
	for (DAT_DIM_T i = 0; i < d; ++i) {
		score += dat_i[i] * w[i];
	}
	score += b;
	return score;
}

COMP_T LabelTree::
LabelTreeNode::compute_norm() const {
	COMP_T norm = 0;
	for (DAT_DIM_T i = 0; i < d; ++i) {
		norm += w[i] * w[i];
	}
	return norm;
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
LabelTreeNode::update_decre_score(COMP_T eta, COMP_T lambda,
								   COMP_T* dat_i, N_DAT_T n) {
	COMP_T term_1 = 1 - eta * lambda;
	COMP_T term_2 = eta;
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
	COMP_T term_2 = eta;
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
LabelTree::LabelTree(unsigned int _nary,
					 DAT_DIM_T 	  _d,
			  		 SUPV_T       _s_labelset,
					 COMP_T 	  _lambda,
			  		 char         _verbosity,
        	  		 COMP_T       _eta0,
        	  		 unsigned int _n_epoch,
        	  		 COMP_T       _eta0_1st_try,
        	  		 COMP_T       _eta0_try_factor)
          :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(_d,
          										   _verbosity,
          										   _eta0,
          										   _n_epoch,
          										   _eta0_1st_try,
          										   _eta0_try_factor),
           lambda(_lambda),
           n_nary(_nary),
           path(NULL),
           depth(NULL),
           s_labelset(_s_labelset),
           path_up_to_date(false) {
    if (dim>0 && s_labelset>0 && n_nary>0) {
    	root_ = new LabelTreeNode(_d, _nary, _s_labelset);
    	score = new COMP_T[_nary];
    }
    else {
    	root_ = NULL;
    	score = NULL;
    }
}

LabelTree::LabelTree(LabelTree& tree)
		  :SGD<COMP_T, SUPV_T, DAT_DIM_T, N_DAT_T>(tree),
		   lambda(tree.lambda),
		   n_nary(tree.n_nary),
		   s_labelset(tree.s_labelset),
		   path_up_to_date(tree.path_up_to_date) {
	if (tree.root_) {
		// duplicate all the tree nodes
		root_ = new LabelTreeNode(*tree.root_);
		std::queue<LabelTreeNode*> breadth_first_traverse;
		std::queue<LabelTreeNode*> breadth_first_traverse_dup;
		LabelTreeNode*  cur, * cur_dup;
		LabelTreeNode** cur_ch;
		breadth_first_traverse.push(tree.root_);
		breadth_first_traverse_dup.push(root_);
		while (!breadth_first_traverse.empty()) {
			cur = breadth_first_traverse.front();
			unsigned int cur_n_ch = cur->num_of_children();
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
	else {
		root_ = NULL;
	}
	if (tree.path && tree.depth) {
		// duplicate the cache
		path  = new unsigned int* [s_labelset];
		depth = new SUPV_T[s_labelset];
		if (path_up_to_date) {
			std::memcpy(depth, tree.depth, sizeof(SUPV_T) * s_labelset);
			for (SUPV_T i = 0; i < s_labelset; ++i) {
				path[i] = new unsigned int [depth[i]];
				std::memcpy(path[i], tree.path[i],
							sizeof(unsigned int) * depth[i]);
			}
		}
		
	}
	else {
		path  = NULL;
		depth = NULL;
	}
	if (tree.score) {
		score = new COMP_T[n_nary];
	}
	else {
		score = NULL;
	}
}

/**** Destructor ****/
LabelTree::~LabelTree() {
	// delete all the tree nodes
	if (root_) {
		std::queue<LabelTreeNode*> breadth_first_traverse;
		breadth_first_traverse.push(root_);
		while (!breadth_first_traverse.empty()) {
			LabelTreeNode* cur      = breadth_first_traverse.front();
			unsigned int   cur_n_ch = cur->num_of_children();
			if (cur_n_ch) {
				LabelTreeNode** cur_ch = cur->children();
				for (unsigned int i = 0; i < cur_n_ch; i++) {
					breadth_first_traverse.push(cur_ch[i]);
				}
			}
			breadth_first_traverse.pop();
			delete cur;
		}
	}
	// delete cache
	if (path) {
		for (SUPV_T i = 0; i < s_labelset; ++i) {
			delete[] path[i];
		}
	}
	delete[] path;
	delete[] depth;
	delete[] score;
}

LabelTree& LabelTree::construct() {
	if (!dim || !s_labelset || !n_nary) {
		std::cerr << "WARNING: LabelTree: Dimensionality or size of the label "
				  << "set or the number of branches of the tree has not been "
                  << "specified. Constructing process is skipped." << std::endl;
        return *this;
	}
	if (!root_) root_ = new LabelTreeNode(dim, n_nary, s_labelset);
    if (!score) score = new COMP_T[n_nary];
    path_up_to_date = false;
    return *this;
}

void LabelTree::rand_data_index(N_DAT_T* index, SUPV_T* y, N_DAT_T n) {
// WARNING: current implementation can generate random integers only less than 
// maximum value of type "int" because of the restrictions of rand() from C 
// standard library.
	SUPV_T* s_dat_of_label  = new SUPV_T[s_labelset];
	COMP_T* p_dat_of_label  = new COMP_T[s_labelset];
	COMP_T* st_dat_of_label = new COMP_T[s_labelset];
	std::forward_list<N_DAT_T>* candidate = 
		new std::forward_list<N_DAT_T>[s_labelset];
	std::memset(s_dat_of_label, 0, sizeof(SUPV_T) * (s_labelset));
	std::memset(p_dat_of_label, 0, sizeof(COMP_T) * (s_labelset));
	for (N_DAT_T i = 0; i < n; ++i) {
		++s_dat_of_label[y[i]-1];
		candidate[y[i]-1].push_front(i);
	}
	SUPV_T most_dat_of_label  = s_dat_of_label[0];
	for (SUPV_T i = 1; i < s_labelset; ++i) {
		if (s_dat_of_label[i] > most_dat_of_label) {
			most_dat_of_label = s_dat_of_label[i];
		}
	}
	for (SUPV_T i = 0; i < s_labelset; ++i) {
		st_dat_of_label[i] = (COMP_T)s_dat_of_label[i] / most_dat_of_label;
	}
	N_DAT_T count = 0;
	while (count < n) {
		for (SUPV_T i = 0; i < s_labelset; ++i) {
			p_dat_of_label[i] += st_dat_of_label[i];
			if (p_dat_of_label[i] >= 1) {
				if (!candidate[i].empty()) {
					N_DAT_T rand_i = std::rand() % s_dat_of_label[i];
					typename std::forward_list<N_DAT_T>::iterator it0 = 
			            candidate[i].before_begin();
			        typename std::forward_list<N_DAT_T>::iterator it1 = 
			            candidate[i].begin();
			        for (N_DAT_T i = 0; i < rand_i; ++i) {
			            ++it0;
			            ++it1;
			        }
			        index[count++] = *it1;
			        candidate[i].erase_after(it0);
			        --s_dat_of_label[i];
			    }
			    --p_dat_of_label[i];
			}
		}
	}

	delete[] s_dat_of_label;
	delete[] p_dat_of_label;
	delete[] st_dat_of_label;
	delete[] candidate;
}

LabelTree& LabelTree::train_one(COMP_T* dat, N_DAT_T i, N_DAT_T n, SUPV_T* y) {
	COMP_T* 	   dat_i      = dat + dim * i;
	LabelTreeNode* start_node = root_;
	unsigned int   n_child    = start_node->num_of_children();
	COMP_T		   max_loss   = 0;

	if (!path_up_to_date) update_path();
	unsigned int*  path_y 	  = path[y[i] - 1];
	SUPV_T		   step_count = 0;
	LabelTreeNode* node_to_incre_score;
	LabelTreeNode* node_to_decre_score;

	while (n_child) {
		LabelTreeNode** child = start_node->children();
		std::memset(score, 0, sizeof(COMP_T) * n_child);
		for (unsigned int i = 0; i < n_child; ++i) {
			score[i] = child[i]->compute_score(dat_i);
		}
		unsigned int path_y_i = path_y[step_count];
		--score[path_y_i];			// is this needed ?
		unsigned int score_i = max(score, n_child);
		if (score_i != path_y_i) {
			COMP_T cur_loss = score[score_i] - score[path_y_i];
			if (cur_loss > max_loss) {
				node_to_decre_score = child[score_i];
				node_to_incre_score  = child[path_y_i];
				max_loss = cur_loss;
			}
		}
		start_node = child[path_y_i];
		step_count++;
		n_child = start_node->num_of_children();
	}

	iterator it = begin();
	for (++it; &it; ++it) {
		if (&it == node_to_decre_score) {
			it->update_decre_score(eta, lambda, dat_i, n);
		}
		else if (&it == node_to_incre_score) {
			it->update_incre_score(eta, lambda, dat_i, n);
		}
		else {
			it->update_no_loss(eta, lambda);
		}
	}

	return *this;
}

SUPV_T LabelTree::test_one(COMP_T* dat_i) const {
	LabelTreeNode* node    = root_;
	unsigned int   n_child = node->num_of_children();
	while (n_child) {
		LabelTreeNode** child = node->children();
		std::memset(score, 0, sizeof(COMP_T) * n_child);
		for (unsigned int i = 0; i < n_child; ++i) {
			score[i] = child[i]->compute_score(dat_i);
		}
		unsigned int i = max(score, n_child);
		node           = child[i];
		n_child        = node->num_of_children();
	}
	SUPV_T* labelset = node->labels();
	return labelset[0];
}

COMP_T LabelTree::compute_obj(COMP_T* dat, N_DAT_T n, SUPV_T* y) {
	COMP_T* dat_i    = dat;
	COMP_T  loss     = 0;
	COMP_T  regul    = 0;
	COMP_T	max_loss = 0;
	if (!path_up_to_date) update_path();
	for (N_DAT_T i = 0; i < n; ++i, dat_i += dim) {
		SUPV_T	      y_i      = y[i];
		unsigned int* path_y_i = path[y_i - 1];
		SUPV_T		  step_count = 0;
		LabelTreeNode* node = root_;
		unsigned int n_child = node->num_of_children();
		while (n_child) {
			LabelTreeNode** child = node->children();
			std::memset(score, 0, sizeof(COMP_T) * n_child);
			for (unsigned int j = 0; j < n_child; ++j) {
				score[j] = child[j]->compute_score(dat_i);
			}
			unsigned int path_y_i_step = path_y_i[step_count];
			--score[path_y_i_step];		// is this needed ?
			unsigned int j = max(score, n_child);
			if (j != path_y_i_step) {
				COMP_T cur_loss = score[j] - score[path_y_i_step];
				max_loss = cur_loss > max_loss ? cur_loss : max_loss;
			}
			node = child[path_y_i_step];
			step_count++;
			n_child = node->num_of_children();
		}
		loss += max_loss;
	}
	iterator it = begin();
	for (++it; &it; ++it) {
		regul += it->compute_norm();
	}
	return lambda * regul + loss / n;
}

LabelTree& LabelTree::ostream_this(std::ostream& out) {
	out << "Tree Nodes are as follows (start from root):";
	for (iterator i = begin(); i != end(); ++i) {
		out << "\n";
		i->ostream_this(out);
	}
	if (path_up_to_date) {
		for (SUPV_T i = 0; i < s_labelset; ++i) {
			out << "\nPath for label " << i + 1 << "\n";
			unsigned int* path_i = path[i];
			out << std::setw(3) << path_i[0];
			for (SUPV_T j = 1; j < depth[i]; ++j) {
				out << " -> " << std::setw(3) << path_i[j];
			}
		}
	}
	return *this;
}

LabelTree& LabelTree::ostream_param(std::ostream& out) {
	static LabelTreeNode* node;
	if (!node) {
		iterator it = begin();
		for (SUPV_T i = 0; i < 3; ++i, ++it);
		node = &it;
	}
	COMP_T* w   = node->weight();
	for (DAT_DIM_T i = 0; i < dim; ++i) {
		out << w[i] << " ";
	}
	out << node->bias();
	return *this;
}

unsigned int LabelTree::max(COMP_T* array, unsigned int length) const {
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

void LabelTree::update_path() {
	bool nothing = !path || !depth;
	if (nothing) {
		path  = new unsigned int* [s_labelset];
		depth = new SUPV_T[s_labelset];
	}
	std::memset(depth, 0, sizeof(SUPV_T) * s_labelset);
	
	std::queue<LabelTreeNode*> breadth_first_traverse;
	breadth_first_traverse.push(root_);
	while (!breadth_first_traverse.empty()) {
		LabelTreeNode* node = breadth_first_traverse.front();
		unsigned int n_child = node->num_of_children();
		if (n_child) {
			LabelTreeNode** child = node->children();
			for (unsigned int i = 0; i < n_child; ++i) {
				SUPV_T  _s_labelset = child[i]->num_of_labels();
				SUPV_T* labelset    = child[i]->labels();
				for (SUPV_T j = 0; j < _s_labelset; ++j) {
					depth[labelset[j] - 1]++;
				}
				breadth_first_traverse.push(child[i]);
			}
		}
		breadth_first_traverse.pop();
	}

	for (SUPV_T i = 0; i < s_labelset; ++i) {
		if (!nothing) delete[] path[i];
		path[i] = new unsigned int [depth[i]];
	}

	SUPV_T* step_count = new SUPV_T[s_labelset];
	std::memset(step_count, 0, sizeof(SUPV_T) * s_labelset);
	breadth_first_traverse.push(root_);
	while (!breadth_first_traverse.empty()) {
		LabelTreeNode* node = breadth_first_traverse.front();
		unsigned int n_child = node->num_of_children();
		if (n_child) {
			LabelTreeNode** child = node->children();
			for (unsigned int i = 0; i < n_child; ++i) {
				SUPV_T  _s_labelset = child[i]->num_of_labels();
				SUPV_T* labelset    = child[i]->labels();
				for (SUPV_T j = 0; j < _s_labelset; ++j) {
					path[labelset[j] - 1][step_count[labelset[j] - 1]++] = i;
				}
				breadth_first_traverse.push(child[i]);
			}
		}
		breadth_first_traverse.pop();
	}

	path_up_to_date = true;
	delete[] step_count;
}

void LabelTree::gdb_output() { ostream_this(std::cout); }