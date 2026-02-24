#pragma once
#include <filesystem>
#include <string>
#include <vector>

struct Config {
	std::filesystem::path m_input_dir;
	std::filesystem::path m_output_dir;
	std::vector<std::string> filename_masks;

	static Config load(const std::filesystem::path& config_path);
};

struct Record {
	uint64_t receive_ts;
	double price;
};


