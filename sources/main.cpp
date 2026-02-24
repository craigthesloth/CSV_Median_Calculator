#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h> 
#include <filesystem>
#include "core/Application.h"

//to start logging from the beginning of the main function
//I had to implement as function in the main.cpp

void setupLogging(const std::filesystem::path& exeDir) {
    std::filesystem::path logDir = exeDir / "resources" / "logs";
    std::filesystem::create_directories(logDir);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        (logDir / "app.log").string(), 5 * 1024 * 1024, 3); //no more than 3 files, each < 5MB
    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
    auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::err);
}

int main(int argc, char* argv[]) {
    try {
        setupLogging(std::filesystem::path(argv[0]).parent_path());
        auto app = createApplication();
        return app->run(argc, argv);
    }
    catch (const std::exception& e) {
        spdlog::error("Unhandled exception: {}", e.what());
        return 1;
    }
}