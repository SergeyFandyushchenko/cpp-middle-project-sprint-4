#pragma once

#include "metric.hpp"

#include <string>

namespace analyzer::metric::metric_impl {

struct CyclomaticComplexityMetric final : IMetric {
    static inline const std::string kName = "Cyclomatic Complexity";

protected:
    std::string Name() const override;

    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
};

}  // namespace analyzer::metric::metric_impl
