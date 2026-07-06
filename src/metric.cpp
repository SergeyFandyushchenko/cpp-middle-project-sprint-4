#include "metric.hpp"

#include "function.hpp"

#include <ranges>

namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyzer::metric {
void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) { metrics.push_back(std::move(metric)); }

/**
 * @brief Вычисляет все зарегистрированные метрики для заданной функции.
 *
 * Эта функция применяет каждый метрический объект из контейнера `metrics`
 * к переданной функции `func` и собирает результаты в вектор.
 */
MetricResults MetricExtractor::Get(const function::Function &func) const {
    return metrics | rv::transform([&func](const auto &metric) { return metric->Calculate(func); }) |
           rs::to<MetricResults>();
}

}  // namespace analyzer::metric
