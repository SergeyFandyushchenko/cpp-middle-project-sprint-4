#include "metric_accumulator_impl/sum_average_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl {

void SumAverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    if (std::holds_alternative<int>(metric_result.value)) {
        sum += std::get<int>(metric_result.value);
        count++;
    }
}

void SumAverageAccumulator::Finalize() {
    if (count > 0) {
        average = static_cast<double>(sum) / count;
    } else {
        average = 0.0;
    }
    is_finalized = true;
}

void SumAverageAccumulator::Reset() {
    is_finalized = false;
    sum = 0;
    count = 0;
    average = 0;
}

SumAverageAccumulator::SumAverage SumAverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("SumAverageAccumulator::Get() called before Finalize()");
    return {sum, average};
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl