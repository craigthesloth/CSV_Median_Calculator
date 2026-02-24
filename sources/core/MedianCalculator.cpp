#include "MedianCalculator.h"
#include <queue>
#include <vector>
#include <functional>
#include <cmath>
#include <spdlog/spdlog.h>

void BoostMedianCalculator::process(const std::vector<Record>& records, MedianChangeCallback on_change) {
    std::priority_queue<double> max_heap; //lower half
    std::priority_queue<double, std::vector<double>, std::greater<double>> min_heap; //upper half
    double last_median = 0.0;
    bool first = true;

    for (const auto& rec : records) {
        //add new element
        if (max_heap.empty() || rec.price <= max_heap.top()) {
            max_heap.push(rec.price);
        }
        else {
            min_heap.push(rec.price);
        }

        //balance
        if (max_heap.size() > min_heap.size() + 1) {
            min_heap.push(max_heap.top());
            max_heap.pop();
        }
        else if (min_heap.size() > max_heap.size()) {
            max_heap.push(min_heap.top());
            min_heap.pop();
        }

        //median
        double current_median;
        if (max_heap.size() == min_heap.size()) {
            current_median = (max_heap.top() + min_heap.top()) / 2.0;
        }
        else {
            current_median = max_heap.top();
        }

        if (first || std::abs(current_median - last_median) > 1e-12) {
            on_change(rec.receive_ts, current_median);
            last_median = current_median;
            first = false;
        }
    }
}