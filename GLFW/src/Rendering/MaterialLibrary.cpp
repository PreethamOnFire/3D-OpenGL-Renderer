#include <iostream>
#include <cstdlib>
#include "MaterialLibrary.h"

Material& MaterialLibrary::create(const std::string& name, ShaderPipeline& pipeline) {
	auto material = std::make_unique<Material>(&pipeline);
	Material& ref = *material;
	materials[name] = std::move(material);
	return ref;
}

Material* MaterialLibrary::get(const std::string& name) const {
	auto it = materials.find(name);
	return (it != materials.end()) ? it->second.get() : nullptr;
}

bool MaterialLibrary::exists(const std::string& name) const {
	return materials.find(name) != materials.end();
}

Material& MaterialLibrary::getOrDefault(const std::string& name) const {
	if (Material* mat = get(name)) return *mat;
	if (Material* def = get("default")) return *def;
	std::cerr << "ERROR: MaterialLibrary has no '" << name << "' or fallback 'default' material!" << std::endl;
	std::abort();
}
