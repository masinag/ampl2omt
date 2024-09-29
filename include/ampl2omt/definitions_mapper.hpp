//
// Created by gabriele on 29/09/24.
//

#ifndef AMPL2OMT_DEFINITIONS_MAPPER_HPP
#define AMPL2OMT_DEFINITIONS_MAPPER_HPP

#include <unordered_set>
#include "mp/expr-visitor.h"
#include "mp/problem.h"


class DefinitionsGetter : public mp::ExprVisitor<DefinitionsGetter, void> {

    std::vector<bool> has_definition_;
    const mp::Problem *p_;

public:
    template<typename T>
    std::vector<int> get(const mp::Problem &p, const mp::BasicProblem<>::Range<T> &terms);

    // TODO: recursive visit
    void VisitUnsupported(mp::Expr e) {};

    void VisitCommonExpr(CommonExpr e);

private:
    void reset_definitions();

    void set_problem(const mp::Problem &p);
};

template<typename T>
std::vector<int>
DefinitionsGetter::get(const mp::Problem &p, const mp::BasicProblem<>::Range<T> &terms) {
    set_problem(p);
    reset_definitions();
    for (const auto &t: terms) {
        Visit(t.nonlinear_expr());
    }
    // note: problem common expressions should be in topological order
    std::vector<int> definitions;
    definitions.reserve(p.num_common_exprs());
    for (int i = 0; i < p.num_common_exprs(); ++i) {
        if (has_definition_[i]) {
            definitions.push_back(i);
        }
    }
    return definitions;
}

class DefinitionsMapper {
    std::vector<int> defs_;
    std::string prefix_;

public:
    template<typename T>
    DefinitionsMapper(const mp::Problem &p, const mp::BasicProblem<>::Range<T> &terms, const std::string &prefix);

    [[nodiscard]] std::vector<int> defs() const;

    [[nodiscard]] std::string def(int i) const;

};

template<typename T>
DefinitionsMapper::DefinitionsMapper(const mp::Problem &p, const mp::BasicProblem<>::Range<T> &terms,
                                     const std::string &prefix) : prefix_(prefix) {
    DefinitionsGetter getter;
    defs_ = getter.get(p, terms);
}

#endif //AMPL2OMT_DEFINITIONS_MAPPER_HPP
