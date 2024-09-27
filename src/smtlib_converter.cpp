#include "smtlib_converter.hpp"

namespace expr = mp::expr;

class SmtLibConverter;


SmtLibConverter::SmtLibConverter(fmt::MemoryWriter &out, mp::Problem &p) : out_(out), p_(p) {}

void SmtLibConverter::VisitUnsupported(mp::Expr e) {
    std::string parsed = out_.str();
    std::string op = expr::str(e.kind());
    throw std::runtime_error("Parsed: " + parsed + "\n"
                                                   "Unsupported expression: " + op);
}

void SmtLibConverter::VisitNumericConstant(mp::NumericConstant c) {
    out_ << float_to_smtlib(c.value());
}

void SmtLibConverter::VisitVariable(mp::Reference v) {
    out_ << var_name(p_.var(v.index()));
}

void SmtLibConverter::VisitCommonExpr(mp::Reference e) {
    auto exp = p_.common_expr(e.index());
    algebraic_expression_to_smtlib(exp, p_, *this);
}

void SmtLibConverter::VisitUnary(mp::UnaryExpr e) {
    VisitUnary(e, expr::str(e.kind()));
}

void SmtLibConverter::VisitMinus(mp::UnaryExpr e) {
    VisitUnary(e, "-");
}

void SmtLibConverter::VisitBinary(mp::BinaryExpr e) {
    std::string op = expr::str(e.kind());
    out_ << "(" << op << " ";
    Visit(e.lhs());
    out_ << " ";
    Visit(e.rhs());
    out_ << ")";
}

void SmtLibConverter::VisitIf(mp::IfExpr e) {
    out_ << "(ite ";
    Visit(e.condition());
    out_ << " ";
    Visit(e.then_expr());
    out_ << " ";
    Visit(e.else_expr());
    out_ << ")";
}

void SmtLibConverter::VisitPLTerm(mp::PLTerm e) {
    auto arg = e.arg();
    out_ << "(* ";
    Visit(arg);
    out_ << " ";
    for (int i = 0; i < e.num_breakpoints(); i++) {
        out_ << "\n\t(ite (<= ";
        Visit(arg);
        out_ << " " << e.breakpoint(i) << ") " << e.slope(i);
    }
    out_ << " " << e.slope(e.num_breakpoints()) << ")";
    for (int i = 0; i < e.num_breakpoints(); i++) {
        out_ << ")";
    }
}

void SmtLibConverter::VisitVarArg(VarArgExpr e) {
    std::string op = expr::str(e.kind());
    VisitNary(e, op);
}

void SmtLibConverter::VisitSum(SumExpr e) {
    VisitNary(e, "+");
}

void SmtLibConverter::VisitRelational(mp::RelationalExpr e) {
    std::string op = expr::str(e.kind());
    out_ << "(" << op << " ";
    Visit(e.lhs());
    out_ << " ";
    Visit(e.rhs());
    out_ << ")";
}

void SmtLibConverter::VisitUnary(mp::UnaryExpr e, const std::string &op) {
    out_ << "(" << op << " ";
    Visit(e.arg());
    out_ << ")";
}

template<typename T>
void SmtLibConverter::VisitNary(const T &e, const std::string &op) {
    out_ << "(" << op;
    for (const auto &arg: e) {
        out_ << " ";
        Visit(arg);
    }
    out_ << ")";
}

std::string float_to_smtlib(double d) {
    std::string template_str;
    if (d == static_cast<int>(d)) {
        template_str = "{:.1f}";
    } else {
        template_str = "{}";
    }

    if (d < 0) {
        return fmt::format("(- {})", fmt::format(template_str, -d));
    } else {
        return fmt::format(template_str, d);
    }
}

std::string var_name(const mp::BasicProblem<>::Variable &v) {
    return fmt::format("x{}", v.index());
}

void constraint_to_smtlib(const mp::BasicProblem<>::AlgebraicCon &con, mp::Problem &p, SmtLibConverter &converter) {
    fmt::MemoryWriter &w = converter.writer();
    bool has_lower_bound = con.lb() != -INFINITY;
    bool has_upper_bound = con.ub() != INFINITY;

    if (has_lower_bound && has_upper_bound) {
        if (con.lb() == con.ub()) {
            w << "(assert (= ";
            algebraic_expression_to_smtlib(con, p, converter);
            w << " " << con.lb() << "))";
        } else {
            w << "(assert (let ((.def_0 ";
            algebraic_expression_to_smtlib(con, p, converter);
            w << ")) (and (<= " << con.lb() << " .def_0) (<= .def_0 " << con.ub() << "))))";
        }
    } else if (has_lower_bound) {
        w << "(assert (<= " << con.lb() << " ";
        algebraic_expression_to_smtlib(con, p, converter);
        w << "))";
    } else if (has_upper_bound) {
        w << "(assert (<= ";
        algebraic_expression_to_smtlib(con, p, converter);
        w << " " << con.ub() << "))";
    }
}

void
objective_to_smtlib(const mp::BasicProblem<>::Objective &obj, mp::Problem &p, SmtLibConverter &converter) {
    fmt::MemoryWriter &w = converter.writer();
    std::string sense = obj.type() == mp::obj::MIN ? "minimize" : "maximize";
    w << "(" << sense << "\n\t";
    algebraic_expression_to_smtlib(obj, p, converter);
    w << "\n)";
}

void variable_decl_to_smtlib(const mp::BasicProblem<>::Variable &v, fmt::MemoryWriter &w, mp::Problem &p) {
    w << "(declare-const " << var_name(v) << " Real)";
    bool has_lower_bound = v.lb() != -INFINITY;
    bool has_upper_bound = v.ub() != INFINITY;

    if (has_lower_bound && has_upper_bound && v.lb() == v.ub()) {
        w << "\n(assert (= " << var_name(v) << " " << float_to_smtlib(v.lb()) << "))";
    } else if (has_lower_bound) {
        w << "\n(assert (<= " << float_to_smtlib(v.lb()) << " " << var_name(v) << "))";
    } else if (has_upper_bound) {
        w << "\n(assert (<= " << var_name(v) << " " << float_to_smtlib(v.ub()) << "))";
    }
}

template<typename T>
void algebraic_expression_to_smtlib(T &expr, mp::Problem &p, SmtLibConverter &converter) {
    fmt::MemoryWriter &w = converter.writer();
    w << "(+";

    bool has_linear = expr.linear_expr().num_terms() > 0;
    bool has_nonlinear = expr.nonlinear_expr();
    if (has_linear) {
        for (const auto &lin_term: expr.linear_expr()) {
            if (lin_term.coef() != 0) {
                w << " (* " << float_to_smtlib(lin_term.coef()) << " " << var_name(p.var(lin_term.var_index())) << ")";
            }
        }
    }
    if (has_nonlinear) {
        if (has_linear) {
            w << " ";
        }
        converter.Visit(expr.nonlinear_expr());
    }
    w << ")";
}
