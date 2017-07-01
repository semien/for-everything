#pragma once
#include "Heap.h"

class Heap : public IHeap
{
public:
	Heap(int N);
	~Heap();
	void push(int value);
	int min();
	void extractMin();
private:
	int hsize;
	int *a_;
};
