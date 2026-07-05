#include "metric_accumulator_impl/average_accumulator.hpp"

namespace analyzer::metric_accumulator::metric_accumulator_impl {

void AverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    // Извлекаем int из variant
    if (std::holds_alternative<int>(metric_result.value)) {
        sum += std::get<int>(metric_result.value);
        count++;
    }
    // Если строка, игнорируем (для categorical метрик)
}

void AverageAccumulator::Finalize() {
    if (count > 0) {
        average = static_cast<double>(sum) / count;
    } else {
        average = 0.0;
    }
    is_finalized = true;
}

void AverageAccumulator::Reset() {
    is_finalized = false;
    sum = 0;
    count = 0;
    average = 0;
}

double AverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("AverageAccumulator::Get() called before Finalize()");
    return average;
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl