#include "FileScanner.h"
#include <spdlog/spdlog.h>

std::vector<std::filesystem::path> FileScanner::scan(
    const std::filesystem::path& directory,
    const std::vector<std::string>& masks)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        spdlog::error("Directory does not exist: {}", directory.string());
        return result;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;
        auto path = entry.path();
        if (path.extension() != ".csv") continue;

        std::string filename = path.filename().string();
        if (masks.empty()) {
            result.push_back(path);
        }
        else {
            for (const auto& mask : masks) {
                if (filename.find(mask) != std::string::npos) {
                    result.push_back(path);
                    break;
                }
            }
        }
    }
    spdlog::info("Found {} CSV files", result.size());
    return result;
}