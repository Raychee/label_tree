#ifndef _LIST_H
#define _LIST_H

#include <iostream>
using std::cout;
using std::endl;

template <class _T> class ListNode {
public:
	ListNode(_T data);
	~ListNode();
	_T data();
	void set_data(_T data);
	ListNode<_T>* next();
	void set_next(ListNode<_T>* next);
private:
	_T data_;
	ListNode<_T>* next_;
};

/* Partial specialized class ListNode */
template <class _T> class ListNode<_T *> {
public:
	ListNode(_T* data);
	~ListNode();
	_T* data();
	void set_data(_T* data);
	ListNode<_T *>* next();
	void set_next(ListNode<_T *>* next);
private:
	_T* data_;
	ListNode<_T*>* next_;
};

template <typename _L, class _T> class List {
public:
	List();
	List(_L label);
	List(_L label, _T data);
	~List();
	ListNode<_T>* head();
	long length();
	_L label();
	void add(ListNode<_T>* node);
	void add(_T data);
private:
	ListNode<_T>* head_;
	ListNode<_T>* tail_;
	long length_;
	_L label_;
};

/***** Class ListNode *****/

template <class _T>
ListNode<_T>::ListNode(_T data):
			  data_(data), next_(NULL) {
	cout << "Constructor: ListNode<_T>(_T data_ = " << data << ")" << endl;
}

template <class _T>
ListNode<_T>::~ListNode() {
	cout << "Destructor: ListNode<_T>(_T data_ = " << data_ << ")" << endl;
}

template <class _T>
inline _T ListNode<_T>::data()
{ return data_; }

template <class _T>
inline void ListNode<_T>::set_data(_T data)
{ data_ = data; }

template <class _T>
inline ListNode<_T> *ListNode<_T>::next()
{ return next_; }

template <class _T>
inline void ListNode<_T>::set_next(ListNode<_T>* next)
{ next_ = next; }



/***** Class ListNode (partial specialized) *****/

template <class _T>
ListNode<_T*>::ListNode(_T* data):
				data_(data), next_(NULL) {
	cout << "Constructor: ListNode<_T *>(_T *data_ = " << data << ")" << endl;
}

template <class _T>
ListNode<_T*>::~ListNode() {
	delete data_;
	cout << "Destructor: ListNode<_T>(_T data_ = " << data_ << ")" << endl;
}

template <class _T>
inline _T* ListNode<_T*>::data()
{ return data_; }

template <class _T>
inline void ListNode<_T*>::set_data(_T* data)
{ data_ = data; }

template <class _T>
inline ListNode<_T*>* ListNode<_T*>::next()
{ return next_; }

template <class _T>
inline void ListNode<_T*>::set_next(ListNode<_T*>* next)
{ next_ = next; }



/****** Class List Function definition ******/

template <typename _L, class _T>
List<_L, _T>::List(): 
			  head_(NULL), tail_(NULL), length_(0), label_(NULL) {}

template <typename _L, class _T>
List<_L, _T>::List(_L label): 
			  head_(NULL), tail_(NULL), length_(0), label_(label) {
	cout << "Constructor: List<_L, _T>(_L label_ = " << label << ")" << endl;
}

template <typename _L, class _T>
List<_L, _T>::List(_L label, _T data):
			  length_(1), label_(label) {
	head_ = tail_ = new ListNode<_T>(data);
	cout << "Constructor: List<_L, _T>(_L label_ = " << label
		 << ", _T data_ = " << data << ")" << endl;
}

template <typename _L, class _T>
List<_L, _T>::~List() {
	ListNode<_T>* ptr = head_;
	ListNode<_T>* ptr_next;
	while (ptr) {
		ptr_next = ptr->next();
		delete ptr;
		ptr = ptr_next;
	}
	cout << "Destructor: List<_L, _T>(_L label_ = " << label_ << ")" << endl;
}

template <typename _L, class _T>
inline ListNode<_T>* List<_L, _T>::head()
{ return head_; }

template <typename _L, class _T>
inline long List<_L, _T>::length()
{ return length_; }

template <typename _L, class _T>
inline _L List<_L, _T>::label()
{ return label_; }

template <typename _L, class _T>
inline void List<_L, _T>::add(ListNode<_T>* node) {
	if (length_) {
		tail_->set_next(node);
		tail_ = node;
	}
	else {
		head_ = tail_ = node;
	}
	length_++;
}

template <typename _L, class _T>
inline void List<_L, _T>::add(_T data) {
	ListNode<_T>* node = new ListNode<_T>(data);
	if (length_) {
		tail_->set_next(node);
		tail_ = node;
	}
	else {
		head_ = tail_ = node;
	}
	length_++;
}

/*
typedef struct node {
	void *data;
	struct node *next;
} NODE;

typedef struct {
	NODE *head;
	NODE *tail;
	long length;
	int type;
	double label;
} LIST;
*/

/* Operations on linked lists */
/*
LIST *create_empty_list(int, double);
LIST *create_list(int, double, void *);
NODE *create_node(int, void *);
void add_node(LIST *, NODE *);
void add_to_list(LIST *, void *);
void free_list(LIST *);
*/

#endif
