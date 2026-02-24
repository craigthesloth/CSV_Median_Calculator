#pragma once
#include "../interfaces/IOutputWriter.hpp"
#include <fstream>
#include <iomanip>

class CsvOutputWriter : public IOutputWriter {
public:
    void open(const std::filesystem::path& path) override;
    void writeHeader() override;
    void writeRow(uint64_t timestamp, double median) override;
    void close() override;

private:
    std::ofstream m_stream;
};