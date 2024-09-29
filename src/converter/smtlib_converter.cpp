#include "ampl2omt/smtlib_converter.hpp"
#include "ampl2omt/smtlib_term_converter.hpp"

SmtLibConverter::SmtLibConverter(fmt::MemoryWriter &out) : w_(out), term_converter_(w_) {}

void SmtLibConverter::convert(mp::Problem &p) {

    SmtLibTermConverter converter(w_);

    w_ << "(set-logic QF_NRAT)\n";
    w_ << "(set-option :produce-models true)\n";
    w_ << "\n";
    declare_variables(p);
    w_ << "\n";
    assert_variables_bounds(p);
    w_ << "\n";
    assert_constraints(p);
    w_ << "\n";
    assert_objectives(p);
    w_ << "(check-sat)\n(get-objectives)\n(exit)\n";
}

void SmtLibConverter::declare_variables(const mp::Problem &p) {
    for (const auto &v: p.vars()) {
        declare_variable(v);
        w_ << "\n";
    }
}

void SmtLibConverter::declare_variable(const mp::BasicProblem<>::Variable &v) {
    w_ << "(declare-const " << var_name(v.index()) << " Real)";
}

void SmtLibConverter::assert_variables_bounds(const mp::Problem &p) {
    std::vector<std::string> var_names;
    var_names.reserve(p.num_vars());
    for (const auto &v: p.vars()) {
        var_names.push_back(var_name(v.index()));
    }
    assert_bounds(p.vars(), var_names);
}

void SmtLibConverter::assert_constraints(const mp::Problem &p) {
    DefinitionsMapper def_mapper(p, p.algebraic_cons(), "con");
    assert_definitions(p, def_mapper);
    for (const auto &con: p.algebraic_cons()) {
        assert_constraint(con);
    }
}

void SmtLibConverter::assert_constraint(const mp::BasicProblem<>::AlgebraicCon &con) {

}

void SmtLibConverter::assert_objectives(const mp::Problem &p) {
    for (const auto &obj: p.objs()) {
        assert_objective(obj);
    }
}

void SmtLibConverter::assert_objective(const mp::BasicProblem<>::Objective &obj) {

}

void SmtLibConverter::assert_definitions(const mp::Problem &p,
                                         const DefinitionsMapper &def_mapper) {
    // ( let ( ⟨var_binding ⟩+ ) ⟨term⟩ )
    term_converter_.set_def_mapper(def_mapper);
    for (const auto &i: def_mapper.defs()) {
        w_ << "(let (" << def_mapper.def(i) << " ";
        algebraic_expression_to_smtlib(p.algebraic_con(i), term_converter_);
        w_ << ")\n";
    }
}

template<typename T>
void SmtLibConverter::assert_bounds(const mp::BasicProblem<>::Range<T> &bounds, const std::vector<std::string> &names) {
    bool multiple = bounds.begin() != bounds.end() && std::next(bounds.begin()) != bounds.end();
    if (multiple) {
        w_ << "(and\n";
    }
    int i = 0;
    for (const auto &b: bounds) {
        w_ << "\t";
        assert_bound(b, names[i]);
        w_ << "\n";
        i++;
    }
    if (multiple) {
        w_ << ")";
    }
    w_ << ")";

}

template<typename T>
void SmtLibConverter::assert_bound(const T &bound, const std::string &name) {
    bool has_lower_bound = bound.lb() != -INFINITY;
    bool has_upper_bound = bound.ub() != INFINITY;

    if (has_lower_bound && has_upper_bound && bound.lb() == bound.ub()) {
        w_ << "(= " << var_name(bound.index()) << " " << float_to_smtlib(bound.lb()) << ")";
    } else if (has_lower_bound) {
        w_ << "(<= " << float_to_smtlib(bound.lb()) << " " << var_name(bound.index()) << ")";
    } else if (has_upper_bound) {
        w_ << "(<= " << var_name(bound.index()) << " " << float_to_smtlib(bound.ub()) << ")";
    }
}