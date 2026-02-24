#include "CsvRecordReader.h"
#include <boost/tokenizer.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <format>

std::vector<Record> CsvRecordReader::read(const std::filesystem::path& file) {
    std::vector<Record> records;
    std::ifstream f(file);
    if (!f.is_open()) {
        spdlog::error(std::format("Cannot open file: {}", file.string()));
        return records;
    }

    std::string line;
    if (!std::getline(f, line)) {
        spdlog::warn(std::format("File is empty: {}", file.string()));
        return records;
    }

    int line_num = 1;
    while (std::getline(f, line)) {
        ++line_num;
        if (line.empty()) continue;

        boost::char_separator<char> sep(";");
        boost::tokenizer<boost::char_separator<char>> tokens(line, sep);
        std::vector<std::string> fields(tokens.begin(), tokens.end());

        if (fields.size() < 3) {
            spdlog::warn(std::format("Line {} in {} has insufficient fields", line_num, file.string()));
            continue;
        }

        try {
            uint64_t ts = std::stoull(fields[0]);
            double price = std::stod(fields[2]);
            records.push_back({ ts, price });
        }
        catch (const std::exception& e) {
            spdlog::error(std::format("Parse error at line {}: {}", line_num, e.what()));
        }
    }
    spdlog::info(std::format("Read {} records from {}", records.size(), file.string()));
    return records;
}