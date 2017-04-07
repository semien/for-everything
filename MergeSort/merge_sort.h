#pragma once
#include<iostream>
#include<vector>

template <class RandomAccessIterator, class Comparator>
void MergeSort1(RandomAccessIterator start, RandomAccessIterator finish, Comparator compare, std::vector<typename RandomAccessIterator::value_type>& buffer)
{	
	if (finish - start>1)
	{
		int size = (finish - start);
		int size_2 = size / 2;
		MergeSort1(start, start + size_2, compare, buffer);
		MergeSort1(start + size_2, finish, compare, buffer);

		int left = 0, right = size_2, i = 0;

		while (left<size_2 && right<size)
		{
			if (compare(start[left], start[right]))
			{
				buffer[i++] = start[left];
				left++;
			}
			else
			{
				buffer[i++] = start[right];
				right++;
			}
			if (left == size_2)
			{
				while (right<size)
				{
					buffer[i++] = start[right];
					right++;
				}
			}
			if (right == size)
			{
				while (left<size_2)
				{
					buffer[i++] = start[left];
					left++;
				}
			}
		}
		for (i = 0; i<size; i++)
			start[i] = buffer[i];
	}
}

template <class RandomAccessIterator, class Comparator>
void MergeSort(RandomAccessIterator start, RandomAccessIterator finish, Comparator compare) {
	std::vector<typename RandomAccessIterator::value_type> buffer(finish - start);
	MergeSort1(start, finish, compare, buffer);
}

template <class RandomAccessIterator>
void MergeSort(RandomAccessIterator first, RandomAccessIterator last) {
	MergeSort(first, last, std::less<typename RandomAccessIterator::value_type>());
};