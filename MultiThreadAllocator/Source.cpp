#include<iostream>
#include<vector>
#include<mutex>
#include<unordered_map>
#include<stack>
#include<thread>

extern void* mtalloc(size_t bytes);
extern void mtfree(void* ptr);

std::mutex mtx_;
std::stack<void*> mem;
void func() {
	for (int i = 0; i < 10; ++i) {
		std::cout << i << " " << std::this_thread::get_id() << "in\n";
		std::vector<void*> vect;
		for (int j = 0; j < 1000; ++j) {
			vect.push_back(mtalloc(50 * sizeof(int)));
		}
		mtx_.lock();
		mem.push(mtalloc(50 * sizeof(int)));
		mtx_.unlock();
		std::cout << i << " " << std::this_thread::get_id() << "free\n";
		for (int j = 0; j < 1000; ++j) {
			mtfree(vect[j]);
		}
		mtx_.lock();
		mtfree(mem.top());
		mem.pop();
		mtx_.unlock();
		std::cout << i << " " << std::this_thread::get_id() << "out\n";
	}
	std::cout << "\nfinished\n";
}

int main() {
	std::thread t1(func);
	std::thread t2(func);
	std::thread t3(func);
	std::thread t4(func);
	std::thread t5(func);
	std::thread t6(func);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	return 0;
}