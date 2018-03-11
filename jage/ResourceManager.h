#pragma once

#include <memory>
#include <string>
#include <map>

#include "AbstractFactory.h"
#include "Log.h"

// Allows deffered creating of resources
class ResourceManager
{
public:
	// Initializes helper objects
	/// Currently does nothing)
	static void init() {}
		
	// Loads information about all assets from file
	static void init(const std::string& path);

	// Clears up all resources
	static void close();

	// Attaches resource factory to specified name
	// T - AbstractFactory child class type
	// Args - AbstractFactory child class constructor arguments
	template <class T, class... Args>
	static void bind(const std::string& name, Args&&... args)
	{
		static_assert(std::is_base_of<AbstractFactory, T>::value,
			"Template parameter of function ResourceManager::bind must be a child class of ResourceFactory class");

		std::unique_ptr<AbstractFactory> factory = std::make_unique<T>(std::forward<Args>(args)...);

		auto key = std::make_pair(name, factory->getStoredTypeIndex());

		auto it = m_factories.find(key);
		if (it == m_factories.end()) {
			factory->m_assignedName = name;
			m_factories[key] = std::move(factory);
		}
		else {
			Log::write("Same resource was already binded: \"" + name + "\", \"" + key.second.name() + "\"");
		}
	}

	// Detaches resource factory from specified name
	// T - Stored type
	template <class T>
	static void unbind(const std::string& name)
	{
		auto key = std::make_pair(name, std::type_index(typeid(T)));

		auto it = m_factories.find(key);
		if (it != m_factories.end()) {
			m_factories.erase(it);
		}
		else {
			Log::write("There wasn't any resource to unbind with name: \"" + name + "\", \"" + key.second.name() + "\"");
		}
	}

	// Returns resource of specified name and type
	// If there is no such resource nullptr will be returned
	// T - Stored type
	template <class T>
	static T* get(const std::string& name)
	{
		auto key = std::make_pair(name, std::type_index(typeid(T)));
		
		auto it = m_factories.find(key);
		if (it == m_factories.end()) {
			throw std::runtime_error("Unable to get resource: \"" + name + "\", \"" + key.second.name() + "\"");
		}
		else {
			return reinterpret_cast<T*>(it->second->load());
		}
	}

	// Clear specified resource, but don't delete it from map
	template <class T>
	static void clear(const std::string& name)
	{
		auto key = std::make_pair(name, std::type_index(typeid(T)));

		auto it = m_factories.find(key);
		if (it != m_factories.end()) {
			return it->second->clear();
		}
	}

private:
	static std::map<std::pair<std::string, std::type_index>, std::unique_ptr<AbstractFactory>> m_factories;
};
