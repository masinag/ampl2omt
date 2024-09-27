//
// Created by gabriele on 27/09/24.
//

#ifndef AMPL2OMT_SMTLIB_CONVERTER_HPP
#define AMPL2OMT_SMTLIB_CONVERTER_HPP

#include "mp/problem.h"
#include "mp/expr-visitor.h"
#include <stdexcept>
#include <string>

/**
 * A visitor that converts an algebraic expression to the SMT-LIB format
 */
class SmtLibConverter : public mp::ExprVisitor<SmtLibConverter, void> {
private:
    fmt::MemoryWriter &out_;
    mp::Problem &p_;

public:
    /**
     * Create a new SmtLibConverter
     * @param out the writer
     * @param p the NL problem
     */
    explicit SmtLibConverter(fmt::MemoryWriter &out, mp::Problem &p);

    fmt::MemoryWriter &writer() { return out_; }

    void VisitUnsupported(Expr e);

    void VisitNumericConstant(NumericConstant c);

    void VisitVariable(Variable v);

    void VisitCommonExpr(CommonExpr e);

    void VisitUnary(UnaryExpr e);

    void VisitMinus(UnaryExpr e);

    void VisitBinary(BinaryExpr e);

    void VisitIf(IfExpr e);


    void VisitPLTerm(PLTerm e);

    void VisitVarArg(VarArgExpr e);

    void VisitSum(SumExpr e);

    void VisitRelational(RelationalExpr e);

private:
    void VisitUnary(UnaryExpr e, const std::string &op);

    template<typename T>
    void VisitNary(const T &e, const std::string &op);
};

/**
 * Convert a double to a string in SMT-LIB format
 * @param d the double to convert
 * @return the string representation
 */
std::string float_to_smtlib(double d);

/**
 * Get the name of a variable
 * @param v the variable
 * @return the name of the variable
 */
std::string var_name(const mp::BasicProblem<>::Variable &v);

/**
 * Convert an algebraic expression to the SMT-LIB format
 * @tparam T the type of the expression
 * @param expr the expression to convert
 * @param p the NL problem
 * @param converter the converter
 */
template<typename T>
void algebraic_expression_to_smtlib(T &expr, mp::Problem &p, SmtLibConverter &converter);

/**
 * Convert a variable declaration to the SMT-LIB format
 * @param v the variable
 * @param w the writer
 * @param p the NL problem
 */
void variable_decl_to_smtlib(const mp::BasicProblem<>::Variable &v, fmt::MemoryWriter &w, mp::Problem &p);

/**
 * Convert an objective to the SMT-LIB format
 * @param obj the objective
 * @param p the NL problem
 * @param converter the converter
 */
void
objective_to_smtlib(const mp::BasicProblem<>::Objective &obj, mp::Problem &p, SmtLibConverter &converter);

/**
 * Convert a constraint to the SMT-LIB format
 * @param con the constraint
 * @param p the NL problem
 * @param converter the converter
 */
void constraint_to_smtlib(const mp::BasicProblem<>::AlgebraicCon &con, mp::Problem &p, SmtLibConverter &converter);


#endif //AMPL2OMT_SMTLIB_CONVERTER_HPP
