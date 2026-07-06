#include <gtest/gtest.h>

#include "metric_impl/naming_style.hpp"

#include <memory>

namespace analyzer::metric::metric_impl::test {

class NamingStyleTest : public ::testing::Test {
protected:
    void SetUp() override { metric = std::make_unique<NamingStyleMetric>(); }

    void TestFunctionName(const std::string &name, const std::string &expected) {
        analyzer::function::Function func;
        func.name = name;
        auto result = metric->Calculate(func);
        EXPECT_EQ(std::get<std::string>(result.value), expected);
    }

    std::unique_ptr<NamingStyleMetric> metric;
};

TEST_F(NamingStyleTest, SnakeCaseTest) {
    TestFunctionName("test_function", "Snake Case");
    TestFunctionName("my_variable_name", "Snake Case");
    TestFunctionName("test_123", "Snake Case");
    TestFunctionName("__test_multiparameters__", "Snake Case");
}

TEST_F(NamingStyleTest, PascalCaseTest) {
    TestFunctionName("TestFunction", "Pascal Case");
    TestFunctionName("MyClass", "Pascal Case");
    TestFunctionName("TestLoops", "Pascal Case");
    TestFunctionName("Testnestedif", "Pascal Case");
}

TEST_F(NamingStyleTest, CamelCaseTest) {
    TestFunctionName("testFunction", "Camel Case");
    TestFunctionName("myVariable", "Camel Case");
}

TEST_F(NamingStyleTest, LowerCaseTest) {
    TestFunctionName("test", "Lower Case");
    TestFunctionName("myvar", "Lower Case");
}

TEST_F(NamingStyleTest, UnknownTest) {
    TestFunctionName("test-function", "Unknown");
    TestFunctionName("Test_Function", "Unknown");
}

TEST_F(NamingStyleTest, FromFilesTest) {
    // Проверяем функции из реальных файлов
    analyzer::file::File file("if.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "testIf"; });
    ASSERT_NE(it, functions.end());
    auto result = metric->Calculate(*it);
    EXPECT_EQ(std::get<std::string>(result.value), "Camel Case");
}

}  // namespace analyzer::metric::metric_impl::test
