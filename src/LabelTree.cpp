# include <iostream>
# include <iomanip>
# include <queue>
# include "LabelTree.h"

using std::ostream;
using std::setw;
using std::setiosflags;
using std::cerr;
using std::endl;
using std::queue;

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


/**** Friend functions ****/
ostream& operator<<(ostream& out, LabelTree& tree) {
	queue<LabelTreeNode*> pipeline;
	LabelTreeNode* cur;
	LabelTreeNode** cur_ch;
	long* labels, n_labels;
	int cur_n_ch;
	out << "Tree Nodes are as follows (start from root):";
	pipeline.push(tree.root_);
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
