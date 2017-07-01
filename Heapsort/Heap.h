#pragma once

class IHeap
{
public:
	virtual void push(int value) = 0;
	virtual int min() = 0;
	virtual void extractMin() = 0;
	virtual ~IHeap() {}
};
