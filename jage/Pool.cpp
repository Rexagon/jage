#include "Pool.h"

BasePool::BasePool(size_t elementSize, size_t chunkSize) :
	m_elementSize(elementSize), m_chunkSize(chunkSize), m_size(0), m_capacity(0)
{
}

BasePool::~BasePool()
{
	for (auto chunk : m_chunks) {
		delete[] chunk;
	}
}

void BasePool::expand(size_t n)
{
	if (n >= m_size) {
		if (n >= m_capacity) {
			reserve(n);
		}

		m_size = n;
	}
}

void BasePool::reserve(size_t n)
{
	while (m_capacity < n)
	{
		char* chunk = new char[m_elementSize * m_chunkSize];
		m_chunks.push_back(chunk);
		m_capacity += m_chunkSize;
	}
}

void * BasePool::get(size_t n)
{
	if (n < m_size) {
		return m_chunks[n / m_chunkSize] + (n % m_chunkSize) * m_elementSize;
	}
	return nullptr;
}

const void * BasePool::get(size_t n) const
{
	if (n < m_size) {
		return m_chunks[n / m_chunkSize] + (n % m_chunkSize) * m_elementSize;
	}
	return nullptr;
}

size_t BasePool::getSize() const
{
	return m_size;
}

size_t BasePool::getCapacity() const
{
	return m_capacity;
}

size_t BasePool::getChunkCount() const
{
	return m_chunks.size();
}
