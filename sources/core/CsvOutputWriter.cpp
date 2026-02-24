#include "CsvOutputWriter.h"
#include <spdlog/spdlog.h>

void CsvOutputWriter::open(const std::filesystem::path& path) {
    m_stream.open(path);
    if (!m_stream.is_open()) {
        throw std::runtime_error("Cannot create output file: " + path.string());
    }
    spdlog::info("Output file opened: {}", path.string());
}

void CsvOutputWriter::writeHeader() {
    m_stream << "receive_ts;price_median\n";
}

void CsvOutputWriter::writeRow(uint64_t timestamp, double median) {
    m_stream << timestamp << ";" << std::fixed << std::setprecision(8) << median << "\n";
}

void CsvOutputWriter::close() {
    m_stream.close();
}