#pragma once
#include "../interfaces/IRecordReader.hpp"
#include "../structures/CommonStructures.hpp"

class CsvRecordReader : public IRecordReader {
public:
    std::vector<Record> read(const std::filesystem::path& file) override;
};