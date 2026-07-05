#pragma once

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

#include <algorithm>
#include <ranges>
#include <string>
#include <vector>

namespace analyzer {

/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */
auto AnalyseFunctions(const std::vector<std::string> &files,
                      const analyzer::metric::MetricExtractor &metric_extractor) {
    // Создаем файлы и извлекаем функции
    auto functions = files |
                     std::views::transform([](const std::string &filename) { return analyzer::file::File(filename); }) |
                     std::views::transform([](const analyzer::file::File &file) {
                         analyzer::function::FunctionExtractor extractor;
                         return extractor.Get(file);
                     }) |
                     std::views::join | std::ranges::to<std::vector<analyzer::function::Function>>();

    // Вычисляем метрики для каждой функции
    return functions | std::views::transform([&metric_extractor](const analyzer::function::Function &func) {
               auto metrics = metric_extractor.Get(func);
               return std::make_pair(func, metrics);
           }) |
           std::ranges::to<std::vector>();
}

/**
 *
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
auto SplitByClasses(const auto &analysis) {
    // Фильтруем только методы классов и группируем по классам
    return analysis | std::views::filter([](const auto &elem) { return elem.first.class_name.has_value(); }) |
           std::views::chunk_by([](const auto &a, const auto &b) { return a.first.class_name == b.first.class_name; }) |
           std::ranges::to<std::vector>();
}

/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
auto SplitByFiles(const auto &analysis) {
    // Группируем по файлам
    return analysis |
           std::views::chunk_by([](const auto &a, const auto &b) { return a.first.filename == b.first.filename; }) |
           std::ranges::to<std::vector>();
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */
void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyzer::metric_accumulator::MetricsAccumulator &accumulator) {
    std::ranges::for_each(analysis, [&accumulator](const auto &elem) {
        if (!elem.second.empty()) {
            accumulator.AccumulateNextFunctionResults(elem.second);
        }
    });
}

}  // namespace analyzer