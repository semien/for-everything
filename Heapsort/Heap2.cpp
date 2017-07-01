#include "stdafx.h"
#include "Heap2.h"
#include<iostream>

Heap2::Heap2(int n)
{
	b_ = new int[n];
	hsize = 0;
}
void Heap2::push(int value) {
	b_[hsize] = value;
	++hsize;
	for (int i = hsize - 1; i >0;i--) 
	    if (b_[i]<b_[i-1]) std::swap(b_[i], b_[i - 1]);
	}

int Heap2::min()
{
	return b_[0];
}

void Heap2::extractMin()
{
	std::swap(b_[0], b_[hsize - 1]);
	hsize -= 1;
	for (int i = hsize - 1; i > 0; i--)
		if (b_[i]<b_[i - 1])
			std::swap(b_[i], b_[i - 1]);

}

Heap2::~Heap2(){
	delete b_;
}

