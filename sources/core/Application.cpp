#include "Application.h"
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <format>
#include <future>
#include "structures/CommonStructures.hpp"
#include "multi_thread_module/NewThreadPool.h"
#include <iostream>

Application::Application(
    std::unique_ptr<IConfigLoader> loader
    ,std::unique_ptr<IFileScanner> scanner
    ,std::unique_ptr<IRecordReader> reader
    ,std::unique_ptr<IMedianCalculator> calculator
    ,std::unique_ptr<IOutputWriter> writer)
    : m_configLoader(std::move(loader))
    ,m_fileScanner(std::move(scanner))
    ,m_recordReader(std::move(reader))
    ,m_medianCalculator(std::move(calculator))
    ,m_outputWriter(std::move(writer))
{
}

int Application::run(int argc, char* argv[]) {
    try {
        //1 parse command line
        boost::program_options::options_description desc("Options");
        desc.add_options()
            ("help,h", "produce help")
            ("config,c", boost::program_options::value<std::string>(), "config file")
            ("cfg", boost::program_options::value<std::string>(), "alternative config flag");

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        std::filesystem::path exeDir = std::filesystem::path(argv[0]).parent_path();
        std::filesystem::path configPath;

        if (vm.count("config")) configPath = vm["config"].as<std::string>();
        else if (vm.count("cfg")) configPath = vm["cfg"].as<std::string>();
        else {
            std::filesystem::path resourceConfig = exeDir / "resources" / "configs" / "config.toml";
            if (std::filesystem::exists(resourceConfig)) {
                configPath = resourceConfig;
            }
            else {
                configPath = exeDir / "config.toml";
            }
        }
        //2 load config
        Config cfg;
        try {
            cfg = m_configLoader->load(configPath);
        }
        catch (const std::exception& e) {
            spdlog::error("Failed to load config: {}", e.what());
            return 1;
        }

        //3 scan files
        auto files = m_fileScanner->scan(cfg.m_input_dir, cfg.filename_masks);
        if (files.empty()) {
            spdlog::error("No CSV files found in directory: {}. Exiting.", cfg.m_input_dir.string());
            if (files.empty()) {
                spdlog::warn("No CSV files found in {}.", cfg.m_input_dir.string());
                std::cout << "Do you want to specify another directory? (y/n): ";
                char answer;
                std::cin >> answer;
                if (answer == 'y' || answer == 'Y') {
                    std::cout << "Enter new directory path: ";
                    std::string newDir;
                    std::cin >> newDir;
                    cfg.m_input_dir = newDir;
                    files = m_fileScanner->scan(cfg.m_input_dir, cfg.filename_masks);
                    if (files.empty()) {
                        spdlog::error("Still no files. Exiting.");
                        return 1;
                    }
                }
                else {
                    return 1;
                }
            }
        }

        //4 read all records
        ThreadPool pool(std::thread::hardware_concurrency());
        std::vector<std::future<std::vector<Record>>> futures;
        futures.reserve(files.size());

        for (const auto& file : files) {
            futures.emplace_back(pool.enqueue([this, file] {
                return m_recordReader->read(file);
                }));
        }

        std::vector<Record> allRecords;
        size_t totalRead = 0;
        for (auto& fut : futures) {
            try {
                auto records = fut.get();
                totalRead += records.size();
                allRecords.insert(allRecords.end(),
                    std::make_move_iterator(records.begin()),
                    std::make_move_iterator(records.end()));
            }
            catch (const std::exception& e) {
                spdlog::error("Error reading file: {}", e.what());
            }
        }

        if (allRecords.empty()) {
            spdlog::error("No valid records read from any file. Exiting.");
            return 1;
        }
        spdlog::info("Total records read: {}", totalRead);


        //5 sort by time
        std::sort(allRecords.begin(), allRecords.end(),
            [](const Record& a, const Record& b) { return a.receive_ts < b.receive_ts; });

        //6 prepare output
        try {
            std::filesystem::create_directories(cfg.m_output_dir);
            std::filesystem::path outPath = cfg.m_output_dir / "result.csv";
            m_outputWriter->open(outPath);
        }
        catch (const std::exception& e) {
            spdlog::error("Cannot create output file: {}", e.what());
            return 1;
        }
        m_outputWriter->writeHeader();

        //7 calculate median and write on changes
        m_medianCalculator->process(allRecords,
            [this](uint64_t ts, double median) {
                m_outputWriter->writeRow(ts, median);
            });

        m_outputWriter->close();
        spdlog::info("Done. Results written to {}", cfg.m_output_dir.string());

    }
    catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}