#pragma once

#include <string>
#include <typeindex>

// Base class for resource factories
class AbstractFactory
{
protected:
	// Helper class to make constructor accept generic type
	template<class T>
	struct tag {
		typedef T type;
	};

public:
	template<class T>
	AbstractFactory(tag<T>) :
		m_storedType(std::type_index(typeid(T)))
	{
	}

	virtual ~AbstractFactory() {
		clear();
    }

	virtual void* load() = 0;
	virtual void clear() {}

	std::type_index getStoredTypeIndex() const {
		return m_storedType;
	}

protected:
	friend class ResourceManager;

	std::string m_assignedName;
	std::type_index m_storedType;
};
