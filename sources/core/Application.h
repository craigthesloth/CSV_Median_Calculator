#pragma once
#include <memory>
#include "TomlConfigLoader.h"
#include "FileScanner.h"
#include "CsvRecordReader.h"
#include "MedianCalculator.h"
#include "CsvOutputWriter.h"
#include <spdlog/sinks/stdout_color_sinks.h>

class Application {
public:
    Application(
        std::unique_ptr<IConfigLoader> loader,
        std::unique_ptr<IFileScanner> scanner,
        std::unique_ptr<IRecordReader> reader,
        std::unique_ptr<IMedianCalculator> calculator,
        std::unique_ptr<IOutputWriter> writer
    );

    int run(int argc, char* argv[]);

private:
    std::unique_ptr<IConfigLoader> m_configLoader;
    std::unique_ptr<IFileScanner> m_fileScanner;
    std::unique_ptr<IRecordReader> m_recordReader;
    std::unique_ptr<IMedianCalculator> m_medianCalculator;
    std::unique_ptr<IOutputWriter> m_outputWriter;
};

inline std::unique_ptr<Application> createApplication() {
    auto loader = std::make_unique<TomlConfigLoader>();
    auto scanner = std::make_unique<FileScanner>();
    auto reader = std::make_unique<CsvRecordReader>();
    auto calculator = std::make_unique<BoostMedianCalculator>();
    auto writer = std::make_unique<CsvOutputWriter>();

    return std::make_unique<Application>(
        std::move(loader),
        std::move(scanner),
        std::move(reader),
        std::move(calculator),
        std::move(writer)
    );
}