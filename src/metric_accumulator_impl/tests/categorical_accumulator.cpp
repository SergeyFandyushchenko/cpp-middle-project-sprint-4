#include <gtest/gtest.h>

#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <algorithm>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class CategoricalAccumulatorTest : public ::testing::Test {
protected:
    void SetUp() override { accumulator = std::make_unique<CategoricalAccumulator>(); }

    std::unique_ptr<CategoricalAccumulator> accumulator;
};

TEST_F(CategoricalAccumulatorTest, SingleCategoryTest) {
    metric::MetricResult result{"naming", std::string("Snake Case")};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    auto freq = accumulator->Get();
    EXPECT_EQ(freq.size(), 1);
    EXPECT_EQ(freq.at("Snake Case"), 1);
}

TEST_F(CategoricalAccumulatorTest, MultipleCategoriesTest) {
    std::vector<std::string> categories = {"Snake Case", "Pascal Case", "Snake Case", "Camel Case", "Snake Case"};
    std::ranges::for_each(categories, [&](const auto &cat) {
        metric::MetricResult result{"naming", cat};
        accumulator->Accumulate(result);
    });

    accumulator->Finalize();
    auto freq = accumulator->Get();
    EXPECT_EQ(freq.size(), 3);
    EXPECT_EQ(freq.at("Snake Case"), 3);
    EXPECT_EQ(freq.at("Pascal Case"), 1);
    EXPECT_EQ(freq.at("Camel Case"), 1);
}

TEST_F(CategoricalAccumulatorTest, ResetTest) {
    metric::MetricResult result{"naming", std::string("Snake Case")};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    auto freq = accumulator->Get();
    EXPECT_EQ(freq.size(), 1);

    accumulator->Reset();
    metric::MetricResult result2{"naming", std::string("Pascal Case")};
    accumulator->Accumulate(result2);
    accumulator->Finalize();
    freq = accumulator->Get();
    EXPECT_EQ(freq.size(), 1);
    EXPECT_EQ(freq.at("Pascal Case"), 1);
}

TEST_F(CategoricalAccumulatorTest, EmptyAccumulatorTest) {
    accumulator->Finalize();
    auto freq = accumulator->Get();
    EXPECT_TRUE(freq.empty());
}

TEST_F(CategoricalAccumulatorTest, IgnoresIntValuesTest) {
    metric::MetricResult result{"test", 42};
    accumulator->Accumulate(result);
    accumulator->Finalize();
    auto freq = accumulator->Get();
    EXPECT_TRUE(freq.empty());
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test