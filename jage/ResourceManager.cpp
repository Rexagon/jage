#include "ResourceManager.h"

std::map<std::pair<std::string, std::type_index>, std::unique_ptr<AbstractFactory>> ResourceManager::m_factories;

void ResourceManager::init(const std::string & path)
{
	//TODO: parse config file
}

void ResourceManager::close()
{
	m_factories.clear();
}
