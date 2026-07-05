#include <gtest/gtest.h>

#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <algorithm>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class SumAverageAccumulatorTest : public ::testing::Test {
protected:
    void SetUp() override { accumulator = std::make_unique<SumAverageAccumulator>(); }

    std::unique_ptr<SumAverageAccumulator> accumulator;
};

TEST_F(SumAverageAccumulatorTest, SingleValueTest) {
    metric::MetricResult result{"test", 10};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    auto result_summary = accumulator->Get();
    EXPECT_EQ(result_summary.sum, 10);
    EXPECT_DOUBLE_EQ(result_summary.average, 10.0);
}

TEST_F(SumAverageAccumulatorTest, MultipleValuesTest) {
    std::vector<int> values = {10, 20, 30, 40};
    std::ranges::for_each(values, [&](int val) {
        metric::MetricResult result{"test", val};
        accumulator->Accumulate(result);
    });
    accumulator->Finalize();
    auto result_summary = accumulator->Get();
    EXPECT_EQ(result_summary.sum, 100);
    EXPECT_DOUBLE_EQ(result_summary.average, 25.0);
}

TEST_F(SumAverageAccumulatorTest, ResetTest) {
    metric::MetricResult result{"test", 10};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    auto result_summary = accumulator->Get();
    EXPECT_EQ(result_summary.sum, 10);

    accumulator->Reset();
    metric::MetricResult result2{"test", 20};
    accumulator->Accumulate(result2);
    accumulator->Finalize();
    result_summary = accumulator->Get();
    EXPECT_EQ(result_summary.sum, 20);
    EXPECT_DOUBLE_EQ(result_summary.average, 20.0);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test