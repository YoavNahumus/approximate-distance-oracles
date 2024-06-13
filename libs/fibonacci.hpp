/*Copyright (c) 2010, Robin Message <Robin.Message@cl.cam.ac.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Univsersity of Cambridge nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF CAMBRIDGE OR ROBIN MESSAGE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef FIBONACCI_HPP
#define FIBONACCI_HPP

template <class V> class FibonacciHeap;

template <class V> struct node {
private:
	node<V>* prev;
	node<V>* next;
	node<V>* child;
	node<V>* parent;
	V value;
	int degree;
	bool marked;
public:
	friend class FibonacciHeap<V>;
	node<V>* getPrev();
	node<V>* getNext();
	node<V>* getChild();
	node<V>* getParent();
	V getValue();
	bool isMarked();

	bool hasChildren();
	bool hasParent();
};

template <class V> class FibonacciHeap {
protected:
	node<V>* heap;
public:

	FibonacciHeap();
	virtual ~FibonacciHeap();
	node<V>* insert(V value);
	void merge(FibonacciHeap& other);

	bool isEmpty();

	V getMinimum();

	V removeMinimum();

	void decreaseKey(node<V>* n,V value);

	node<V>* find(V value);
private:
	node<V>* _empty();

	node<V>* _singleton(V value);

	node<V>* _merge(node<V>* a,node<V>* b);

	void _deleteAll(node<V>* n);
	
	void _addChild(node<V>* parent,node<V>* child);

	void _unMarkAndUnParentAll(node<V>* n);

	node<V>* _removeMinimum(node<V>* n);

	node<V>* _cut(node<V>* heap,node<V>* n);

	node<V>* _decreaseKey(node<V>* heap,node<V>* n,V value);

	node<V>* _find(node<V>* heap,V value);
};

#endif