#include<mutex>
#include<iostream>
#include<cmath>
#include<unordered_map>
#include<algorithm>
#include<thread>
#include<cstdlib>
#include<stack>
#include<vector>

const size_t MAX_DEGREE = 14;
const size_t SUPER_SIZE = (size_t)pow(2, MAX_DEGREE);

class Heap {
public:
	Heap(int id) : id_(id) {}
	Heap(Heap&& h) { id_ = h.id_; }
	void* allocate(size_t bytes) {
		int degree = std::max(2, (int)ceil(log2(bytes)));
		std::unique_lock<std::mutex> lock(mtx_);
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
		std::unique_lock<std::mutex> lock(mtx_);
		blocks_[(size_t)ptr[1]].push_back(ptr);
	}
	~Heap() {
		for (auto i : superblocks_)
			free(i);
	}
private:
	std::mutex mtx_;
	int id_;
	std::vector<char*> blocks_[MAX_DEGREE + 1];
	std::vector<void*> superblocks_;
};

class MTAllocator {
public:
	MTAllocator(size_t maxNumThreads = std::thread::hardware_concurrency() * 2) {
		for (size_t i = 0; i < maxNumThreads; ++i)
			heaps_.emplace_back(i);
	}

	void* allocate(size_t bytes) {
		if (bytes + 1 >= SUPER_SIZE) {
			void* ptr = malloc(bytes + 1); // using malloc
			if (ptr == nullptr) {
				return nullptr;
			}
			((char*)ptr)[0] = (char)255;
			return (char*)ptr + 1;
		}
		size_t threadHeap;
		size_t threadID = std::hash<std::thread::id>()(std::this_thread::get_id());
		/*mtx_.lock();
		if (threadID_.count(std::this_thread::get_id()) == 0){
		threadID_[std::this_thread::get_id()] = threadID_.size();
		}
		mtx_.unlock();
		threadHeap = threadID_[std::this_thread::get_id()];*/
		threadHeap = threadID % heaps_.size();
		void* ptr = heaps_[threadHeap].allocate(bytes);
		if (ptr == nullptr) { // if there is not enough memory
			for (size_t i = (threadHeap + 1) % heaps_.size(); i != threadHeap; i = (i + 1) % heaps_.size()) {
				ptr = heaps_[threadHeap].allocate(bytes);
				if (ptr != nullptr) {
					return ptr;
				}
			}
			return nullptr;
		}
		else {
			return ptr;
		}
	}
	void free(void* ptr) {
		if (ptr == nullptr) {
			return;
		}
		if (((char*)ptr)[-1] == (char)255) { // if malloc used
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
	std::mutex mtx_;
};

MTAllocator allocator;

extern void* mtalloc(size_t bytes) {
	return allocator.allocate(bytes);
}

extern void mtfree(void* ptr) {
	allocator.free(ptr);
}