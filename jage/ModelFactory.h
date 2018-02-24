#pragma once

#include <memory>

#include "Model.h"

#include "AbstractFactory.h"

class ModelFactory : public AbstractFactory
{
public:
	ModelFactory(const std::string& filename);

	void* load() override;
	void clear() override;

private:
	std::string m_filename;

	std::unique_ptr<Model> m_data;
};
