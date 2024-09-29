#ifndef AMPL2OMT_SMTLIB_CONVERTER_HPP
#define AMPL2OMT_SMTLIB_CONVERTER_HPP


#include "mp/format.h"
#include "mp/problem.h"
#include "smtlib_term_converter.hpp"
#include "definitions_mapper.hpp"

class SmtLibConverter {
    fmt::MemoryWriter &w_;

    SmtLibTermConverter term_converter_;
public:
    explicit SmtLibConverter(fmt::MemoryWriter &out);

    void convert(mp::Problem &p);

private:
    void declare_variables(const mp::Problem &p);

    void declare_variable(const mp::BasicProblem<>::Variable &v);

    void assert_variables_bounds(const mp::Problem &p);

    void assert_definitions(const mp::Problem &p, const DefinitionsMapper &def_mapper);

    template<typename T>
    void assert_bounds(const mp::BasicProblem<>::Range<T> &bounds, const std::vector<std::string> &names);

    template<typename T>
    void assert_bound(const T &bound, const std::string &name);

    void assert_constraints(const mp::Problem &p);

    void assert_constraint(const mp::BasicProblem<>::AlgebraicCon &con);

    void assert_objectives(const mp::Problem &p);

    void assert_objective(const mp::BasicProblem<>::Objective &obj);
};


#endif //AMPL2OMT_SMTLIB_CONVERTER_HPP
