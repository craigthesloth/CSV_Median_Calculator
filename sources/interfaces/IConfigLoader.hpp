#pragma once
#include <filesystem>
#include "../structures/CommonStructures.hpp"

class IConfigLoader{
public:
	virtual ~IConfigLoader()=  default;
	virtual Config load(const std::filesystem::path& path) = 0;
};