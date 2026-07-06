#include <gtest/gtest.h>

#include "metric_impl/code_lines_count.hpp"

#include <memory>
#include <ranges>

namespace analyzer::metric::metric_impl::test {

class CodeLinesCountTest : public ::testing::Test {
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

        metric = std::make_unique<CodeLinesCountMetric>();
    }

    void TestFunction(const std::string &name, int expected) {
        auto it = std::ranges::find_if(functions, [&name](const auto &f) { return f.name == name; });
        ASSERT_NE(it, functions.end()) << "Function " << name << " not found";
        auto result = metric->Calculate(*it);
        EXPECT_EQ(std::get<int>(result.value), expected) << "Failed for function: " << name;
    }

    std::vector<analyzer::file::File> files;
    std::vector<analyzer::function::Function> functions;
    std::unique_ptr<CodeLinesCountMetric> metric;
};

TEST_F(CodeLinesCountTest, CommentsFileTest) { TestFunction("Func_comments", 3); }

TEST_F(CodeLinesCountTest, ExceptionsFileTest) { TestFunction("Try_Exceptions", 7); }

TEST_F(CodeLinesCountTest, IfFileTest) { TestFunction("testIf", 3); }

TEST_F(CodeLinesCountTest, LoopsFileTest) { TestFunction("TestLoops", 6); }

TEST_F(CodeLinesCountTest, ManyLinesFileTest) { TestFunction("testmultiline", 11); }

TEST_F(CodeLinesCountTest, MatchCaseFileTest) { TestFunction("test_Match_case", 7); }

TEST_F(CodeLinesCountTest, NestedIfFileTest) { TestFunction("Testnestedif", 8); }

TEST_F(CodeLinesCountTest, SimpleFileTest) { TestFunction("test_simple", 5); }

TEST_F(CodeLinesCountTest, TernaryFileTest) { TestFunction("teSt_ternary", 1); }

}  // namespace analyzer::metric::metric_impl::test
