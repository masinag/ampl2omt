//
// Created by gabriele on 27/09/24.
//

#ifndef AMPL2OMT_SMTLIB_TERM_CONVERTER_HPP
#define AMPL2OMT_SMTLIB_TERM_CONVERTER_HPP

#include "mp/problem.h"
#include "mp/expr-visitor.h"
#include "definitions_mapper.hpp"
#include <stdexcept>
#include <string>

/**
 * A visitor that converts an algebraic expression to the SMT-LIB format
 */
class SmtLibTermConverter : public mp::ExprVisitor<SmtLibTermConverter, void> {
private:
    fmt::MemoryWriter &out_;
    const DefinitionsMapper *def_mapper_;

public:
    /**
     * Create a new SmtLibTermConverter
     * @param out the writer
     * @param p the NL problem
     */
    explicit SmtLibTermConverter(fmt::MemoryWriter &out);

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

    void set_def_mapper(const DefinitionsMapper &def_mapper) {
        def_mapper_ = &def_mapper;
    }

private:
    void VisitUnary(UnaryExpr e, const std::string &op);

    template<typename T>
    void VisitNary(T &e, const std::string &op, bool can_squash = false);
};

/**
 * Convert a double to a string in SMT-LIB format
 * @param d the double to convert
 * @return the string representation
 */
std::string float_to_smtlib(double d);

/**
 * Get the name of a variable
 * @param index the index of the variable
 * @return the name of the variable
 */
std::string var_name(int index);

/**
 * Convert an algebraic expression to the SMT-LIB format
 * @tparam T the type of the expression
 * @param expr the expression to convert
 * @param converter the converter
 */
template<typename T>
void algebraic_expression_to_smtlib(const T &expr, SmtLibTermConverter &converter) {
    fmt::MemoryWriter &w = converter.writer();

    std::vector<size_t> linear_terms_idx;
    for (size_t i = 0; i < expr.linear_expr().num_terms(); i++) {
        if (expr.linear_expr().coef(i) != 0) {
            linear_terms_idx.push_back(i);
        }
    }

    bool has_linear = !linear_terms_idx.empty();
    bool has_nonlinear = expr.nonlinear_expr();

    bool is_sum = (linear_terms_idx.size() > 1) ||
                  (has_linear && has_nonlinear);
    if (is_sum) {
        w << "(+";
    }

    for (size_t i: linear_terms_idx) {
        w << " (* " << float_to_smtlib(expr.linear_expr().coef(i)) << " "
          << var_name(expr.linear_expr().var_index(i)) << ")";
    }

    if (has_nonlinear) {
        if (is_sum) {
            w << " ";
        }
        converter.Visit(expr.nonlinear_expr());
    }
    if (is_sum) {
        w << ")";
    }
}

#endif //AMPL2OMT_SMTLIB_TERM_CONVERTER_HPP
