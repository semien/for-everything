#include "stdafx.h"
#include "Heap_impl.h"
#include<iostream>

Heap::Heap(int n) {
	a_ = new int[n];
	hsize = 0;
}
void siftUp(int a[]) {
	while (a[i] > a[(i - 1) / 2]) {
		std::swap(a[i], a[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}



void siftDown()
{
	std::swap(a_[0], a_[hsize-1]); 
	hsize -= 1;
	int i=0;
	while (2 * i + 1 < hsize) {
		int left = 2 * i + 1; 
		int right = 2 * i + 2;
		int j = left;
		if (right < hsize && a_[right] < a_[left])
			j = right;
		if (a_[i] < a_[j]) break;
			std::swap(a_[i], a_[j]);
			i = j;
		
	}
}

	delete a_;
}
Heap::~Heap() {}
