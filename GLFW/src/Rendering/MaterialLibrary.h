#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "Material.h"

class MaterialLibrary {
public:
	Material& create(const std::string& name, ShaderPipeline& pipeline);
	Material* get(const std::string& name) const;
	bool exists(const std::string& name) const;
	Material& getOrDefault(const std::string& name) const;
	const std::unordered_map<std::string, std::unique_ptr<Material>>& getAll() const { return materials; }

private:
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};
