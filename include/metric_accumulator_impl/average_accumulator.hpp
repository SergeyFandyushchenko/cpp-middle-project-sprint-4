#pragma once

#include "metric_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl {

struct AverageAccumulator : IAccumulator {
    void Accumulate(const metric::MetricResult &metric_result) override;

    void Finalize() override;

    void Reset() override;

    double Get() const;

private:
    int sum = 0;
    int count = 0;
    double average = 0;
};

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
