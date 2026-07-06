#pragma once

#include "metric_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl {

struct CategoricalAccumulator : IAccumulator {
    void Accumulate(const metric::MetricResult &metric_result) override;

    void Finalize() override;

    void Reset() override;

    const std::unordered_map<std::string, int> &Get() const;

private:
    std::unordered_map<std::string, int> categories_freq;
};

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
