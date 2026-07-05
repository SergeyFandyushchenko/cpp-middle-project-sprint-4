#include <gtest/gtest.h>

#include "metric_impl/parameters_count.hpp"

#include <memory>
#include <ranges>

namespace analyzer::metric::metric_impl::test {

class ParametersCountTest : public ::testing::Test {
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

        metric = std::make_unique<CountParametersMetric>();
    }

    void TestFunction(const std::string &name, int expected) {
        auto it = std::ranges::find_if(functions, [&name](const auto &f) { return f.name == name; });
        ASSERT_NE(it, functions.end()) << "Function " << name << " not found";
        auto result = metric->Calculate(*it);
        EXPECT_EQ(std::get<int>(result.value), expected) << "Failed for function: " << name;
    }

    std::vector<analyzer::file::File> files;
    std::vector<analyzer::function::Function> functions;
    std::unique_ptr<CountParametersMetric> metric;
};

TEST_F(ParametersCountTest, CommentsFileTest) {
    TestFunction("Func_comments", 3);  // result, a, b
}

TEST_F(ParametersCountTest, ManyParametersFileTest) {
    TestFunction("__test_multiparameters__", 5);  // a, b, c, *args, **kwargs
}

TEST_F(ParametersCountTest, ExceptionsFileTest) {
    TestFunction("Try_Exceptions", 0);  // no parameters
}

TEST_F(ParametersCountTest, IfFileTest) {
    TestFunction("testIf", 1);  // x
}

TEST_F(ParametersCountTest, LoopsFileTest) {
    TestFunction("TestLoops", 1);  // n
}

TEST_F(ParametersCountTest, ManyLinesFileTest) {
    TestFunction("testmultiline", 0);  // no parameters
}

TEST_F(ParametersCountTest, MatchCaseFileTest) {
    TestFunction("test_Match_case", 1);  // x
}

TEST_F(ParametersCountTest, NestedIfFileTest) {
    TestFunction("Testnestedif", 2);  // x, y
}

TEST_F(ParametersCountTest, SimpleFileTest) {
    TestFunction("test_simple", 0);  // no parameters
}

TEST_F(ParametersCountTest, TernaryFileTest) {
    TestFunction("teSt_ternary", 1);  // x
}

}  // namespace analyzer::metric::metric_impl::test