#pragma once
#include <filesystem>
#include <vector>
#include "../structures/CommonStructures.hpp"

class IRecordReader {
public:
    virtual ~IRecordReader() = default;
    virtual std::vector<Record> read(const std::filesystem::path& file) = 0;
};