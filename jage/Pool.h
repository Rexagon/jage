#pragma once

#include <vector>

class BasePool
{
public:
	BasePool(size_t elementSize, size_t chunkSize = 8192);
	virtual ~BasePool();

	void expand(size_t n);
	void reserve(size_t n);
	virtual void destroy(size_t n) = 0;

	void* get(size_t n);
	const void* get(size_t n) const;

	size_t getSize() const;
	size_t getCapacity() const;
	size_t getChunkCount() const;
	
private:
	std::vector<char*> m_chunks;
	size_t m_elementSize;
	size_t m_chunkSize;
	size_t m_size;
	size_t m_capacity;
};


template<typename T, size_t ChunkSize = 8192>
class Pool : public BasePool
{
public:
	Pool() :
		BasePool(sizeof(T), ChunkSize)
	{}

	virtual void destroy(size_t n) override
	{
		if (n < m_size) {
			T* ptr = reinterpret_cast<T*>(get(n));
			ptr->~T();
		}
	}
};