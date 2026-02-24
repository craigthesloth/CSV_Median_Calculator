#pragma once
#include <filesystem>
#include <vector>
#include <string>

class IFileScanner {
public:
    virtual ~IFileScanner() = default;
    virtual std::vector<std::filesystem::path> scan(
        const std::filesystem::path& directory,
        const std::vector<std::string>& masks) = 0;
};