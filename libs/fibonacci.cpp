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
#include "fibonacci.hpp"

template <class V>
node<V>* node<V>::getPrev() {
    return prev;
}

template <class V>
node<V>* node<V>::getNext() {
    return next;
}

template <class V>
node<V>* node<V>::getChild() {
    return child;
}

template <class V>
node<V>* node<V>::getParent() {
    return parent;
}

template <class V>
V node<V>::getValue() {
    return value;
}

template <class V>
bool node<V>::isMarked() {
    return marked;
}

template <class V>
bool node<V>::hasChildren() {
    return child;
}

template <class V>
bool node<V>::hasParent() {
    return parent;
}

template <class V>
FibonacciHeap<V>::FibonacciHeap() {
    heap = _empty();
}

template <class V>
FibonacciHeap<V>::~FibonacciHeap() {
    if(heap) {
        _deleteAll(heap);
    }
}

template <class V>
node<V>* FibonacciHeap<V>::insert(V value) {
    node<V>* ret=_singleton(value);
    heap=_merge(heap,ret);
    return ret;
}

template <class V>
void FibonacciHeap<V>::merge(FibonacciHeap& other) {
    heap=_merge(heap,other.heap);
    other.heap=_empty();
}

template <class V>
bool FibonacciHeap<V>::isEmpty() {
    return heap==nullptr;
}

template <class V>
V FibonacciHeap<V>::getMinimum() {
    return heap->value;
}

template <class V>
V FibonacciHeap<V>::removeMinimum() {
    node<V>* old=heap;
    heap=_removeMinimum(heap);
    V ret=old->value;
    delete old;
    return ret;
}

template <class V>
void FibonacciHeap<V>::decreaseKey(node<V>* n,V value) {
    heap=_decreaseKey(heap,n,value);
}

template <class V>
node<V>* FibonacciHeap<V>::find(V value) {
    return _find(heap,value);
}

template <class V>
node<V>* FibonacciHeap<V>::_empty() {
    return nullptr;
}

template <class V>
node<V>* FibonacciHeap<V>::_singleton(V value) {
    node<V>* n=new node<V>;
    n->value=value;
    n->prev=n->next=n;
    n->degree=0;
    n->marked=false;
    n->child=nullptr;
    n->parent=nullptr;
    return n;
}

template <class V>
node<V>* FibonacciHeap<V>::_merge(node<V>* a,node<V>* b) {
    if(a==nullptr)return b;
    if(b==nullptr)return a;
    if(a->value>b->value) {
        node<V>* temp=a;
        a=b;
        b=temp;
    }
    node<V>* an=a->next;
    node<V>* bp=b->prev;
    a->next=b;
    b->prev=a;
    an->prev=bp;
    bp->next=an;
    return a;
}

template <class V>
void FibonacciHeap<V>::_deleteAll(node<V>* n) {
    if(n!=nullptr) {
        node<V>* c=n;
        do {
            node<V>* d=c;
            c=c->next;
            _deleteAll(d->child);
            delete d;
        } while(c!=n);
    }
}

template <class V>
void FibonacciHeap<V>::_addChild(node<V>* parent,node<V>* child) {
    child->prev=child->next=child;
    child->parent=parent;
    parent->degree++;
    parent->child=_merge(parent->child,child);
}

template <class V>
void FibonacciHeap<V>::_unMarkAndUnParentAll(node<V>* n) {
    if(n==nullptr)return;
    node<V>* c=n;
    do {
        c->marked=false;
        c->parent=nullptr;
        c=c->next;
    }while(c!=n);
}

template <class V>
node<V>* FibonacciHeap<V>::_removeMinimum(node<V>* n) {
    _unMarkAndUnParentAll(n->child);
    if(n->next==n) {
        n=n->child;
    } else {
        n->next->prev=n->prev;
        n->prev->next=n->next;
        n=_merge(n->next,n->child);
    }
    if(n==nullptr)return n;
    node<V>* trees[64]={nullptr};
    
    while(true) {
        if(trees[n->degree]!=nullptr) {
            node<V>* t=trees[n->degree];
            if(t==n)break;
            trees[n->degree]=nullptr;
            if(n->value<t->value) {
                t->prev->next=t->next;
                t->next->prev=t->prev;
                _addChild(n,t);
            } else {
                t->prev->next=t->next;
                t->next->prev=t->prev;
                if(n->next==n) {
                    t->next=t->prev=t;
                    _addChild(t,n);
                    n=t;
                } else {
                    n->prev->next=t;
                    n->next->prev=t;
                    t->next=n->next;
                    t->prev=n->prev;
                    _addChild(t,n);
                    n=t;
                }
            }
            continue;
        } else {
            trees[n->degree]=n;
        }
        n=n->next;
    }
    node<V>* min=n;
    node<V>* start=n;
    do {
        if(n->value<min->value)min=n;
        n=n->next;
    } while(n!=start);
    return min;
}

template <class V>
node<V>* FibonacciHeap<V>::_cut(node<V>* heap,node<V>* n) {
    if(n->next==n) {
        n->parent->child=nullptr;
    } else {
        n->next->prev=n->prev;
        n->prev->next=n->next;
        n->parent->child=n->next;
    }
    n->next=n->prev=n;
    n->marked=false;
    return _merge(heap,n);
}

template <class V>
node<V>* FibonacciHeap<V>::_decreaseKey(node<V>* heap,node<V>* n,V value) {
    if(n->value<value)return heap;
    n->value=value;
    if(n->parent) {
        if(n->value<n->parent->value) {
            heap=_cut(heap,n);
            node<V>* parent=n->parent;
            n->parent=nullptr;
            while(parent!=nullptr && parent->marked) {
                heap=_cut(heap,parent);
                n=parent;
                parent=n->parent;
                n->parent=nullptr;
            }
            if(parent!=nullptr && parent->parent!=nullptr)parent->marked=true;
        }
    } else {
        if(n->value < heap->value) {
            heap = n;
        }
    }
    return heap;
}

template <class V>
node<V>* FibonacciHeap<V>::_find(node<V>* heap,V value) {
    node<V>* n=heap;
    if(n==nullptr)return nullptr;
    do {
        if(n->value==value)return n;
        node<V>* ret=_find(n->child,value);
        if(ret)return ret;
        n=n->next;
    }while(n!=heap);
    return nullptr;
}