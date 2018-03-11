#pragma once

#include <memory>
#include <vector>

class AbstractFileSystem;

class FileManager
{
public:
	// Initializes filesystem with specified FileSystem in template argument
	// T - AbstractFileSystem child class type
	// Args - AbstractFileSystem child class constructor arguments
	template <class T, class... Args>
	static void init(Args&&... args)
	{
		static_assert(std::is_base_of<AbstractFileSystem, T>::value,
			"Template parameter of FileManager::onInit must be a child of AbstractFileSystem class");
		
		m_fileSystem = std::make_unique<T>(std::forward(args)...);
	}

	// Cleares up current filesystem object
	static void close();

	// Tries to open specified file
	// Returns vector of bytes if success
	// Throws std::runtime_error if failed
	static std::string open(const std::string& filename);

private:
	static std::unique_ptr<AbstractFileSystem> m_fileSystem;
};


// Base class for file sytem
class AbstractFileSystem
{
public:
	virtual ~AbstractFileSystem() {}

	virtual std::string open(const std::string& filename) const = 0;
};

// Filesystem which uses OS
class DefaultFileSystem : public AbstractFileSystem
{
public:
	DefaultFileSystem(const std::string& dataFolder = "data/");

	std::string open(const std::string& filename) const override;

private:
	std::string m_dataFolder;
};
