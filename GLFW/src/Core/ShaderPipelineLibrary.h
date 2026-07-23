#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "ShaderPipeline.h"

class ShaderPipelineLibrary {
public:
	ShaderPipeline& load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
	ShaderPipeline* get(const std::string& name) const;
	bool exists(const std::string& name) const;
	const std::unordered_map<std::string, std::unique_ptr<ShaderPipeline>>& getAll() const { return pipelines; }

private:
	std::unordered_map<std::string, std::unique_ptr<ShaderPipeline>> pipelines;
};
