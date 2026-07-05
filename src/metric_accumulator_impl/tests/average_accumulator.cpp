#include <gtest/gtest.h>

#include "metric_accumulator_impl/average_accumulator.hpp"

#include <algorithm>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class AverageAccumulatorTest : public ::testing::Test {
protected:
    void SetUp() override { accumulator = std::make_unique<AverageAccumulator>(); }

    std::unique_ptr<AverageAccumulator> accumulator;
};

TEST_F(AverageAccumulatorTest, SingleValueTest) {
    metric::MetricResult result{"test", 10};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    EXPECT_DOUBLE_EQ(accumulator->Get(), 10.0);
}

TEST_F(AverageAccumulatorTest, MultipleValuesTest) {
    std::vector<int> values = {10, 20, 30, 40, 50};

    std::ranges::for_each(values, [&](auto val) {
        metric::MetricResult result{"test", val};
        accumulator->Accumulate(result);
    });

    accumulator->Finalize();
    EXPECT_DOUBLE_EQ(accumulator->Get(), 30.0);
}

TEST_F(AverageAccumulatorTest, ResetTest) {
    metric::MetricResult result{"test", 10};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    EXPECT_DOUBLE_EQ(accumulator->Get(), 10.0);

    accumulator->Reset();
    metric::MetricResult result2{"test", 20};
    accumulator->Accumulate(result2);
    accumulator->Finalize();
    EXPECT_DOUBLE_EQ(accumulator->Get(), 20.0);
}

TEST_F(AverageAccumulatorTest, EmptyAccumulatorTest) {

    EXPECT_THROW(accumulator->Get(), std::runtime_error);  // AverageAccumulator::Get() called before Finalize()
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test