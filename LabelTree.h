# ifndef _LABELTREE_H
# define _LABELTREE_H


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
	friend std::ostream& operator<<(std::ostream& out, LabelTree& tree);
private:
	LabelTreeNode* root_;
};

/**** inline Methods: LabelTreeNode ****/
inline int LabelTreeNode::nary() const { return n_nary; }
inline long* LabelTreeNode::labels() const { return labelset_; }
inline long LabelTreeNode::num_of_labels() const { return s_labelset_; }
inline LabelTreeNode* LabelTreeNode::parent() const { return parent_; }
inline LabelTreeNode** LabelTreeNode::children() const { return child; }
inline int LabelTreeNode::num_of_children() const { return n_child; }
inline bool LabelTreeNode::is_full_of_children() const {
	return n_child >= n_nary;
}

/**** inline Methods: LabelTree ****/
inline LabelTreeNode* LabelTree::root() const { return root_; }
inline LabelTree& LabelTree::init(int nary, long* labelset, long s_labelset) {
	root_ = new LabelTreeNode(nary, labelset, s_labelset);
	return *this;
}

# endif
