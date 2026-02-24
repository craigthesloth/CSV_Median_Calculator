#pragma once
#include "../interfaces/IFileScanner.hpp"

class FileScanner : public IFileScanner {
public:
    std::vector<std::filesystem::path> scan(
        const std::filesystem::path& directory,
        const std::vector<std::string>& masks) override;
};