#include "stdafx.h"
#include "Heap_impl.h"
#include <fstream>
#include <iostream>
#include <time.h>
#include "Heap2.h"

void heapSort(int *start, int *finish, IHeap *heap) {

	int size = finish - start + 1;
	for (int i = 0; i < size; i++) {
		heap->push(start[i]);
	};
	for (int i = 0; i < size; i++) {
		start[i] = heap->min();
		heap->extractMin();
	}
	
}

void areEqual(IHeap *heapFirst, IHeap* heapSecond) {
   bool test = true;
   int N;
   std::ifstream in("input.txt");
   std::ofstream out("output.txt");
   in >> N;
   int *arr1 = new int[N];
   int *arr2 = new int [N];

   for (int i = 0; i < N; i++) {
	   in >> arr1[i];
	   arr2[i] = arr1[i];
   }
   
    heapSort(arr1,arr1+N-1, heapFirst);
	heapSort(arr2, arr2 + N - 1, heapSecond);
	for (int i = 0; i < N; i++)
		if (arr1[i] != arr2[i]) test = false;
	if (test == true) {
		out << "CORRECTLY!\n";
		for (int i = 0; i < N; i++) out << arr1[i] << " ";
	}
	else {
		out << "WRONG!!!";
	}

}

int main()
{
	int N;
	std::ifstream in("input.txt");
	in >> N;
	
	Heap h1(N);
	Heap2 h2(N);
	areEqual(&h1, &h2);
    return 0;
}

