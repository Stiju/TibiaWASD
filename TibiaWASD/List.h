#ifndef LIST_H
#define LIST_H

template<class T>
class List {
public:
	struct Node {
		T Data;
		Node *Next;
	};

	List() : start(0) {}
	~List() {
		Clear();
	}
	void Clear() {
		Node *p = start, *t;
		while(p) {
			t = p;
			p = p->Next;
			delete t;
		}
		start = 0;
	}
	void Add(const T &data) {
		if(!start) {
			start = new Node();
			start->Data = data;
			start->Next = 0;
		} else {
			Node *p = start;
			while(p->Next)
				p = p->Next;
			p->Next = new Node();
			p->Next->Data = data;
			p->Next->Next = 0;
		}
	}
	Node *Del(Node *node) {
		Node *p = start, *t;
		if(start == node) {
			start = start->Next;
			delete p;
			return start;
		}
		t = p;
		while(p) {
			if(p == node) {
				t->Next = p->Next;
				delete p;
				return t->Next;
			}
			t = p;
			p = p->Next;
		}
		return 0;
	}
	T *Find(const T &data) {
		Node *p = start;
		while(p) {
			if(p->Data == data)
				return &p->Data;
			p = p->Next;
		}
		return 0;
	}
	Node *Begin() { return start; }
	int Count() {
		int c = 0;
		for(Node *p = start; p != 0; p = p->Next)
			c++;
		return c;
	}
private:
	Node *start;
};

#endif