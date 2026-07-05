#include "metric_accumulator_impl/categorical_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl {

void CategoricalAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    if (std::holds_alternative<std::string>(metric_result.value)) {
        categories_freq[std::get<std::string>(metric_result.value)]++;
    }
}

void CategoricalAccumulator::Finalize() { is_finalized = true; }

void CategoricalAccumulator::Reset() {
    is_finalized = false;
    categories_freq.clear();
}

const std::unordered_map<std::string, int> &CategoricalAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("CategoricalAccumulator::Get() called before Finalize()");
    return categories_freq;
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl