#include<vector>
#include<mutex>
#include<iostream>
#include<cmath>
#include<unordered_map>
#include<algorithm>
#include<thread>
#include<cstdlib>

const int MAX_DEGREE = 14;
const int SUPER_SIZE = pow(2,MAX_DEGREE);

class Heap {
public:
	Heap(int id) : id_(id) {}
	Heap(Heap& h) { id_ = h.id_; }
	~Heap() {
		for (auto i : superblocks_)
			free(i);
	}
	void* allocate(size_t bytes) {
		int degree = std::max(3, (int)ceil(log2(bytes)));
		std::unique_lock<std::mutex> lock(mutex_);

		if (sizeClassIsFull(degree)) {
			char* newSb = (char*)malloc(SUPER_SIZE); // allocate new superblock
			if (newSb == nullptr) {
				return nullptr;
			}
			size_t numBlocksInSuperblock = SUPER_SIZE / pow(2, degree);
			for (size_t i = 0; i < numBlocksInSuperblock; ++i) {
				blocks_[degree].push_back(newSb + i*(size_t)pow(2, degree));
			}
			superblocks_.push_back(newSb);
		}

		char* ptr = blocks_[degree].back();
		blocks_[degree].pop_back();
		lock.unlock();
		ptr[0] = id_; // record threadID 
		ptr[1] = degree; // record degree
		return (ptr + 2);
	}

	bool sizeClassIsFull(size_t degree) {
		return blocks_[degree].size() == 0;
	}

	void heapFree(char* ptr) {
		std::unique_lock<std::mutex> lock(mutex_);
		blocks_[ptr[1]].push_back(ptr);
	}
private:
	std::mutex mutex_;
	int id_;
	std::vector<char*> blocks_[MAX_DEGREE + 1];
	std::vector<void*> superblocks_;
};

thread_local size_t threadNumber = std::hash<std::thread::id>()(std::this_thread::get_id());

class MTAllocator {
public:
	MTAllocator(size_t maxNumThreads = std::thread::hardware_concurrency()*2) {
		for (size_t i = 0; i < maxNumThreads; ++i)
			heaps_.emplace_back(i);
	}
	void* allocate(size_t bytes) {
		if (bytes + 2 > SUPER_SIZE) {
			void* ptr = malloc(bytes + 1);
			if (!ptr) {
				return nullptr;
			}
			((char*)ptr)[0] = 255;
			return (char*)ptr + 1;
		}
		void* ans = heaps_[threadNumber % heaps_.size()].allocate(bytes);
		if (ans) { 
			return ans; 
		}
		return nullptr;
	}
	void free(void* ptr) {
		if (((char*)ptr)[-1] == 255) {
			free((char*)ptr - 1);
			return;
		}
		else {
			char* truePtr = ((char*)ptr) - 2;
			heaps_[truePtr[0]].heapFree(truePtr);
		}
	}
private:
	std::vector<Heap> heaps_;
};

/*MTAllocator allocator;

extern void* mtalloc(size_t bytes) {
	return allocator.allocate(bytes);
}

extern void mtfree(void* ptr) {
	allocator.free(ptr);
}*/