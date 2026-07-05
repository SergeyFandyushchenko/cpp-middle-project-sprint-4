#include <gtest/gtest.h>

#include "metric_impl/cyclomatic_complexity.hpp"

#include <memory>
#include <ranges>

namespace analyzer::metric::metric_impl::test {

class CyclomaticComplexityTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::vector<std::string> filenames = {"comments.py",   "exceptions.py",      "if.py",         "loops.py",
                                              "many_lines.py", "many_parameters.py", "match_case.py", "nested_if.py",
                                              "simple.py",     "ternary.py"};

        files = filenames | std::views::transform([](const std::string &name) { return analyzer::file::File(name); }) |
                std::ranges::to<std::vector>();

        // Извлекаем функции из всех файлов
        analyzer::function::FunctionExtractor extractor;
        functions = files | std::views::transform([&extractor](const auto &file) { return extractor.Get(file); }) |
                    std::views::join | std::ranges::to<std::vector>();

        metric = std::make_unique<CyclomaticComplexityMetric>();
    }

    void TestFunction(const std::string &name, int expected) {
        auto it = std::ranges::find_if(functions, [&name](const auto &f) { return f.name == name; });
        ASSERT_NE(it, functions.end()) << "Function " << name << " not found";
        auto result = metric->Calculate(*it);
        EXPECT_EQ(std::get<int>(result.value), expected) << "Failed for function: " << name;
    }

    std::vector<analyzer::file::File> files;
    std::vector<analyzer::function::Function> functions;
    std::unique_ptr<CyclomaticComplexityMetric> metric;
};

TEST_F(CyclomaticComplexityTest, CommentsFileTest) {
    TestFunction("Func_comments", 1);  // base
}

TEST_F(CyclomaticComplexityTest, ExceptionsFileTest) {
    TestFunction("Try_Exceptions", 4);  // base + try + except + finally + assert
}

TEST_F(CyclomaticComplexityTest, IfFileTest) {
    TestFunction("testIf", 2);  // base + 1 if
}

TEST_F(CyclomaticComplexityTest, LoopsFileTest) {
    TestFunction("TestLoops", 4);  // base + for + while + if
}

TEST_F(CyclomaticComplexityTest, ManyLinesFileTest) {
    TestFunction("testmultiline", 2);  // base + assert
}

TEST_F(CyclomaticComplexityTest, MatchCaseFileTest) {
    TestFunction("test_Match_case", 4);  // base + 3 cases
}

TEST_F(CyclomaticComplexityTest, NestedIfFileTest) {
    TestFunction("Testnestedif", 4);  // base + outer if + inner if + elif + else
}

TEST_F(CyclomaticComplexityTest, SimpleFileTest) {
    TestFunction("test_simple", 2);  // base + assert
}

TEST_F(CyclomaticComplexityTest, TernaryFileTest) {
    TestFunction("teSt_ternary", 3);  // base + ternary (conditional_expression) + nested conditional
}

}  // namespace analyzer::metric::metric_impl::test
