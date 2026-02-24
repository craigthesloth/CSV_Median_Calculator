#pragma once
#include <filesystem>
#include <vector>
#include <cstdint>

class IOutputWriter {
public:
    virtual ~IOutputWriter() = default;
    virtual void open(const std::filesystem::path& path) = 0;
    virtual void writeHeader() = 0;
    virtual void writeRow(uint64_t timestamp, double median) = 0;
    virtual void close() = 0;
};