# ifndef _LABELTREE_H
# define _LABELTREE_H

# include <iostream>
# include <queue>


using std::ostream;
using std::setw;
using std::setiosflags;
using std::cerr;
using std::endl;
using std::queue;


class LabelTreeNode {
public:
	LabelTreeNode(int nary);
	LabelTreeNode(int nary, long* labelset, long s_labelset);
	~LabelTreeNode();
	int nary() const;
	long* labels() const;
	long num_of_labels() const;
	LabelTreeNode* parent() const;
	LabelTreeNode** children() const;
	int num_of_children() const;
	bool is_full_of_children() const;
	LabelTreeNode& attach_child(LabelTreeNode* ch);
	LabelTreeNode& attach_child(long* labelset, long s_labelset);
	LabelTreeNode& attach_children(LabelTreeNode** ch, int n_ch);
private:
	const int n_nary;
	double* w;
	long s_labelset_;
	long* labelset_;
	int n_child;
	LabelTreeNode** child;
	LabelTreeNode* parent_;
};


class LabelTree {
public:
	LabelTree() {}
	LabelTree(LabelTreeNode* root);
	LabelTree(int nary);
	~LabelTree();
	LabelTree& init(int nary, long* labelset, long s_labelset);
	LabelTreeNode* root() const;
private:
	LabelTreeNode* root_;
};

ostream& operator<<(ostream& out, LabelTree& tree);


/******** LabelTreeNode functions ********/
/**** Constructor ****/
LabelTreeNode::LabelTreeNode(int nary):
			   n_nary(nary),
			   w(NULL), s_labelset_(0), labelset_(NULL),
			   n_child(0), parent_(NULL) {
	child = new LabelTreeNode*[nary];
}
LabelTreeNode::LabelTreeNode(int nary, long* labelset, long s_labelset):
			   n_nary(nary),
			   w(NULL), s_labelset_(s_labelset), labelset_(labelset),
			   n_child(0), parent_(NULL) {
	child = new LabelTreeNode*[nary];
}
/**** Destructor ****/
LabelTreeNode::~LabelTreeNode() {
	delete w;
	delete[] labelset_;
	delete[] child;
}

/**** Methods ****/
inline int LabelTreeNode::nary() const { return n_nary; }
inline long* LabelTreeNode::labels() const { return labelset_; }
inline long LabelTreeNode::num_of_labels() const { return s_labelset_; }
inline LabelTreeNode* LabelTreeNode::parent() const { return parent_; }
inline LabelTreeNode** LabelTreeNode::children() const { return child; }
inline int LabelTreeNode::num_of_children() const { return n_child; }

inline bool LabelTreeNode::is_full_of_children() const {
	return n_child >= n_nary;
}

LabelTreeNode& LabelTreeNode::attach_child(LabelTreeNode* ch) {
	if (n_child >= n_nary) {
		cerr << "Error while attaching a child: LabelTreeNode is already full."
			 << endl;
		return *this;
	}
	ch->parent_ = this;
	child[n_child] = ch;
	n_child++;
	return *this;
}

LabelTreeNode& LabelTreeNode::attach_child(long* labelset, long s_labelset) {
	if (n_child >= n_nary) {
		cerr << "Error while attaching a child: LabelTreeNode is already full."
			 << endl;
		return *this;
	}
	LabelTreeNode* ch = new LabelTreeNode(n_nary, labelset, s_labelset);
	ch->parent_ = this;
	child[n_child] = ch;
	n_child++;
	return *this;
}

LabelTreeNode& LabelTreeNode::attach_children(LabelTreeNode** ch, int n_ch) {
	if (n_ch >= n_nary) {
		cerr << "Error while attaching children: Too many children. ("
			 << n_nary << "children at most while trying to attach "
			 << n_ch << ")" << endl;
		return *this;
	}
	for (int i = 0; i < n_ch; i++) {
		ch[i]->parent_ = this;
		child[i] = ch[i];
	}
	n_child = n_ch;
	return *this;
}


/******** LabelTree functions ********/
/**** Constructor ****/
LabelTree::LabelTree(LabelTreeNode* root): root_(root) {}
LabelTree::LabelTree(int nary) {
	root_ = new LabelTreeNode(nary);
}
/**** Destructor ****/
LabelTree::~LabelTree() {
	queue<LabelTreeNode*> pipeline;
	LabelTreeNode* cur;
	LabelTreeNode** cur_ch;
	int cur_n_ch;
	pipeline.push(root_);
	while (!pipeline.empty()) {
		cur = pipeline.front();
		cur_ch = cur->children();
		cur_n_ch = cur->num_of_children();
		for (int i = 0; i < cur_n_ch; i++) {
			pipeline.push(cur_ch[i]);
		}
		pipeline.pop();
		delete cur;
	}
}

/**** Methods ****/
LabelTree& LabelTree::init(int nary, long* labelset, long s_labelset) {
	root_ = new LabelTreeNode(nary, labelset, s_labelset);
	return *this;
}
inline LabelTreeNode* LabelTree::root() const { return root_; }


/******** Other functions ********/
ostream& operator<<(ostream& out, LabelTree& tree) {
	queue<LabelTreeNode*> pipeline;
	LabelTreeNode* cur;
	LabelTreeNode** cur_ch;
	long* labels, n_labels;
	int cur_n_ch;
	pipeline.push(tree.root());
	while (!pipeline.empty()) {
		cur = pipeline.front();
		cur_ch = cur->children();
		cur_n_ch = cur->num_of_children();
		labels = cur->labels();
		n_labels = cur->num_of_labels();
		out << "\nLabelTreeNode(" << cur << ") {\n\tparent = \t "
			<< cur->parent() << "\n\tlabels =\t";
		for (int i = 0; i < n_labels; i++) {
			out << " " << setw(3) << setiosflags(std::ios_base::left)
				<< labels[i];
		}
		out << "\n\tchildren =\t";
		for (int i = 0; i < cur_n_ch; i++) {
			out << " " << cur_ch[i];
			pipeline.push(cur_ch[i]);
		}
		out << "\n}";
		pipeline.pop();
	}
	return out;
}

# endif