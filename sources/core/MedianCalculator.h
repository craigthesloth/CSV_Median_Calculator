#pragma once
#include "../interfaces/IMedianCalculator.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

class BoostMedianCalculator : public IMedianCalculator {
public:
    void process(const std::vector<Record>& records, MedianChangeCallback on_change) override;
};