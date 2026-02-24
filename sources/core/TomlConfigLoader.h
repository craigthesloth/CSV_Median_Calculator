// TomlConfigLoader.hpp
#pragma once
#include "../interfaces/IConfigLoader.hpp"

class TomlConfigLoader : public IConfigLoader {
public:
    Config load(const std::filesystem::path& path) override;
};