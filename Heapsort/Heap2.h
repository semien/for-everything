#pragma once
#include "Heap.h"

class Heap2 : public IHeap
{
public:
	Heap2(int N);
	~Heap2();
	void push(int value);
	int min();
	void extractMin();
private:
	int *b_;
	int hsize;
};