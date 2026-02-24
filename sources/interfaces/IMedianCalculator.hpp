#pragma once
#include <vector>
#include <functional>
#include "../structures/CommonStructures.hpp"

using MedianChangeCallback = std::function<void(uint64_t timestamp, double median)>;

class IMedianCalculator {
public:
    virtual ~IMedianCalculator() = default;
    virtual void process(const std::vector<Record>& records, MedianChangeCallback on_change) = 0;
};