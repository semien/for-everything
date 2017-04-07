#include<vector>
#include<iostream>
#include"merge_sort.h"

int main() {
	std::vector<int> vec = { 8,10,3,4,89,345,1 };
	for (int i = 0; i < vec.size(); ++i) {
		std::cout << vec[i] << " ";
	}
	std::cout << std::endl;
	MergeSort(vec.begin(), vec.end(), [](int i, int j) {return i > j; });
	for (int i = 0; i < vec.size(); ++i) {
		std::cout << vec[i] << " ";
	}
	system("pause");
	return 0;
}