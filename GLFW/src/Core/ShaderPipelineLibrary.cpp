#include "ShaderPipelineLibrary.h"

ShaderPipeline& ShaderPipelineLibrary::load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
	auto pipeline = std::make_unique<ShaderPipeline>(vertexPath.c_str(), fragmentPath.c_str());
	ShaderPipeline& ref = *pipeline;
	pipelines[name] = std::move(pipeline);
	return ref;
}

ShaderPipeline* ShaderPipelineLibrary::get(const std::string& name) const {
	auto it = pipelines.find(name);
	return (it != pipelines.end()) ? it->second.get() : nullptr;
}

bool ShaderPipelineLibrary::exists(const std::string& name) const {
	return pipelines.find(name) != pipelines.end();
}
