/**
 * Fibonacci Queue
 * Copyright (c) 2014, Emmanuel Benazera beniz@droidnik.fr, All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * This is basically a Fibonacci heap with an added fast store for retrieving
 * nodes, and decrease their key as needed. Useful for search algorithms (e.g.
 * Dijstra, heuristic, ...).
 */

#ifndef FIBOQUEUE_H
#define FIBOQUEUE_H

#include "fiboheap.h"
#include <unordered_map>
#include <algorithm>

template<class T, class V, class Comp = std::less<T>>
class FibQueue : public FibHeap<T, V, Comp>
{
 public:
  using Heap = FibHeap<T, V, Comp>;
  using Node = typename Heap::FibNode;
  using KeyNodeIter = typename std::unordered_map<int, Node*>::iterator;

  FibQueue()
    : Heap()
    {
    }

  FibQueue(Comp comp)
      : Heap(comp)
  {
  }

  ~FibQueue()
    {
    }

  void decrease_key(Node *x, T k)
  {
    Heap::decrease_key(x,std::move(k));
  }

  Node* push(T k, void *pl, V v)
  {
    Node *x = Heap::push(std::move(k),pl,v);
    fstore.insert({ v, x });
    return x;
  }

  Node* push(T k, V v)
  {
    return push(std::move(k),NULL,v);
  }

  void decrease_key_or_push(T k, V v)
  {
    KeyNodeIter mit = find(v);
    if (mit != fstore.end())
      decrease_key(mit->second,k);
    else
      push(k,v);
  }

  KeyNodeIter find(V v)
  {
    return fstore.find(v);
  }

  int count(V v)
  {
      auto mit = fstore.find(v);
      return mit != fstore.end();
  }

  Node* findNode(V v)
  {
    KeyNodeIter mit = find(v);
    return mit->second;
  }

  V pop()
  {
    if (Heap::empty())
      return -1;
    Node *x = Heap::extract_min();
    if (!x)
      return -1; // should not happen.
    
    V v = x->value;
    fstore.erase(v);
    delete x;
    return v;
  }

  void clear() {
      Heap::clear();
      fstore.clear();
  }

  std::unordered_map<int, Node*> fstore;
};

#endif
