#include "metric_impl/code_lines_count.hpp"

#include "utils.hpp"

#include <ranges>

namespace analyzer::metric::metric_impl {

std::string CodeLinesCountMetric::Name() const { return kName; }

MetricResult::ValueType CodeLinesCountMetric::CalculateImpl(const function::Function &f) const {
    auto &function_ast = f.ast;

    // Вспомогательная лямбда для извлечения номера строки из диапазона узла AST.
    // Формат узла в S-выражении: (node_type [start_line,start_column] [end_line,end_column] ...)
    // Эта функция ищет открывающую скобку "[" после заданной позиции и парсит первую координату — номер строки.
    auto line_number = [&](size_t start_pos) -> int {
        size_t line_pos = function_ast.find("[", start_pos);
        if (line_pos == std::string::npos)
            return 0;
        size_t comma_pos = function_ast.find(",", line_pos);
        if (comma_pos == std::string::npos)
            return 0;
        return ToInt(function_ast.substr(line_pos + 1, comma_pos - line_pos - 1));
    };

    // Определяем начальную и конечную строки тела функции:
    // - начальная строка берётся из корневого узла функции (первое вхождение "[")
    // - конечная строка ищется по шаблону "] -"
    const auto body_start = function_ast.find("body", 0);
    const int start_line = line_number(body_start);
    const int end_line = line_number(function_ast.find("] -"));
    if (body_start == std::string::npos || (start_line == 0 && end_line == 0) || start_line > end_line) {
        return 0;
    }

    std::string_view body = [&](size_t start_pos) {
        auto sv = std::string_view(function_ast).substr(start_pos);
        auto pos = sv.find('\n');
        return sv.substr(pos);
    }(body_start);

    // Лямбда, проверяющая, является ли конкретная строка "кодовой", то есть не комментарием.
    auto is_code_line = [&](int line) {
        std::string line_marker = "[" + std::to_string(line) + ",";
        size_t line_pos = body.find(line_marker);

        if (line_pos == std::string::npos)
            return false;

        size_t node_start = body.rfind('(', line_pos);
        if (node_start == std::string::npos)
            return false;

        std::string_view node_type =
            body.substr(node_start + 1, body.find_first_of(" \n[", node_start + 1) - node_start - 1);

        return node_type != "comment";
    };

    auto line_range = std::views::iota(start_line, end_line + 1);
    auto code_lines = line_range | std::views::filter(is_code_line);

    return static_cast<int>(std::ranges::distance(code_lines));
}

}  // namespace analyzer::metric::metric_impl