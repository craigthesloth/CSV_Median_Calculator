#include "TomlConfigLoader.h"
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

Config TomlConfigLoader::load(const std::filesystem::path& path) {
    Config cfg;
    try {
        auto table = toml::parse_file(path.string());
        auto main_table = table["main"];
        if (!main_table) throw std::runtime_error("Missing [main] section");

        auto input_val = main_table["input"].value<std::string>();
        if (!input_val) throw std::runtime_error("Missing 'input' field");
        cfg.m_input_dir = *input_val;

        if (auto out_val = main_table["output"].value<std::string>())
            cfg.m_output_dir = *out_val;
        else
            cfg.m_output_dir = std::filesystem::current_path() / "output";

        if (auto mask_array = main_table["filename_mask"].as_array()) {
            for (auto&& elem : *mask_array)
                if (auto s = elem.value<std::string>())
                    cfg.filename_masks.push_back(*s);
        }

        spdlog::info("Config loaded from {}", path.string());
        return cfg;
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to load config: {}", e.what());
        throw;
    }
}