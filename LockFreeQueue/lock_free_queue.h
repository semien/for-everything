#pragma once 
#include <atomic> 
#include <thread> 
#include <algorithm> 
#include <atomic> 
#include <thread>

unsigned const MAX_HAZARD_POINTERS = 100;

struct HazardPointer
{
	std::atomic<std::thread::id> id;
	std::atomic<void*> pointer;
};
HazardPointer g_hazardPointers[MAX_HAZARD_POINTERS];

class HPOwner
{
public:
	HPOwner(const HPOwner&) = delete;
	HPOwner operator=(const HPOwner&) = delete;

	HPOwner()
		: hp_(nullptr)
	{
		for (unsigned i = 0; i < MAX_HAZARD_POINTERS; ++i) {
			std::thread::id oldId;
			if (g_hazardPointers[i].id.compare_exchange_strong(
				oldId, std::this_thread::get_id())) {
				hp_ = &g_hazardPointers[i];
				break;
			}
		}
		if (!hp_) {
			throw std::runtime_error("No hazard pointers available");
		}
	}

	std::atomic<void*>& getPointer()
	{
		return hp_->pointer;
	}

	~HPOwner()
	{
		hp_->pointer.store(nullptr);
		hp_->id.store(std::thread::id());
	}

private:
	HazardPointer* hp_;
};

std::atomic<void*>& getHazardPointerForCurrentThread()
{
	thread_local static HPOwner hazard;
	return hazard.getPointer();
}

bool outstandingHazardPointersFor(void* p)
{
	for (unsigned i = 0; i < MAX_HAZARD_POINTERS; ++i) {
		if (g_hazardPointers[i].pointer.load() == p) {
			return true;
		}
	}
	return false;
}

template<typename T>
void doDelete(void* p)
{
	delete static_cast<T*>(p);
}

struct DataToReclaim
{
	void* data;
	std::function<void(void*)> deleter;
	DataToReclaim* next;

	template<typename T>
	DataToReclaim(T* p)
		: data(p)
		, deleter(&doDelete<T>)
		, next(nullptr)
	{}

	~DataToReclaim()
	{
		deleter(data);
	}
};

std::atomic<DataToReclaim*> NodesToReclaim;

void addToReclaimList(DataToReclaim* Node)
{
	Node->next = NodesToReclaim.load();
	while (!NodesToReclaim.compare_exchange_weak(Node->next, Node));
}

template<typename T>
void reclaimLater(T* data)
{
	addToReclaimList(new DataToReclaim(data));
}

void deleteNodesWithNoHazards()
{
	DataToReclaim* current = NodesToReclaim.exchange(nullptr);
	while (current) {
		DataToReclaim* const next = current->next;
		if (!outstandingHazardPointersFor(current->data)) {
			delete current;
		}
		else {
			addToReclaimList(current);
		}
		current = next;
	}
}

template <typename T>
class lock_free_queue {
	struct Node {
		std::atomic<Node *> next;
		T data;
		Node() : next(nullptr) {}
		Node(T const& v) : next(nullptr), data(v) {}
	};
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
public:
	lock_free_queue()
	{
		Node * p = new Node;
		head.store(p);
		tail.store(p);
	}

	~lock_free_queue() {
		while (Node* const oldHead = head.load()) {
			head.store(oldHead->next);
			delete oldHead;
		}
	}

	void Enqueue(T data)
	{
		HPOwner hpo;
		std::atomic<void*>& hp = hpo.getPointer();
		Node* pNew = new Node(data);
		Node* oldTail;
		while (true) {
			oldTail = tail.load();
			hp.store(oldTail);
			if (oldTail != tail.load()) continue;
			Node* next = oldTail->next.load();
			if (oldTail != tail) continue;
			if (next != nullptr) {
				tail.compare_exchange_weak(oldTail, next);
				continue;
			}
			Node* tmp = nullptr;
			if (oldTail->next.compare_exchange_strong(tmp, pNew))
				break;
		}
		tail.compare_exchange_strong(oldTail, pNew);
		hp.store(nullptr);
	}

	bool Dequeue(T& item)
	{
		HPOwner hpo1;
		HPOwner hpo2;
		std::atomic<void*>& hp1 = hpo1.getPointer();
		std::atomic<void*>& hp2 = hpo2.getPointer();
		Node* oldHead;
		while (true) {
			oldHead = head.load();
			hp1.store(oldHead);
			if (oldHead != head.load()) 
				continue;
			Node* oldTail = tail.load();
			Node* next = oldHead->next.load();
			hp2.store(next);
			if (oldHead != head.load())
				continue;
			if (next == nullptr) {
				hp1.store(nullptr);
				return false;
			}
			if (oldHead == oldTail) {
				tail.compare_exchange_strong(oldTail, next);
				continue;
			}
			item = next->data;
			if (head.compare_exchange_strong(oldHead, next))
				break;
		}
		hp1.store(nullptr);
		hp2.store(nullptr);
		if (oldHead) {
			if (outstandingHazardPointersFor(oldHead)) {
				reclaimLater(oldHead);
			}
			else {
				delete oldHead;
			}
			deleteNodesWithNoHazards();
		}
		return true;
	}
};