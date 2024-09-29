#include "ampl2omt/smtlib_term_converter.hpp"

namespace expr = mp::expr;

class SmtLibTermConverter;


SmtLibTermConverter::SmtLibTermConverter(fmt::MemoryWriter &out) : out_(out), def_mapper_(nullptr) {}

void SmtLibTermConverter::VisitUnsupported(mp::Expr e) {
    std::string parsed = out_.str();
    std::string op = expr::str(e.kind());
    throw std::runtime_error("Parsed: " + parsed + "\n"
                                                   "Unsupported expression: " + op);
}

void SmtLibTermConverter::VisitNumericConstant(mp::NumericConstant c) {
    out_ << float_to_smtlib(c.value());
}

void SmtLibTermConverter::VisitVariable(mp::Reference v) {
    out_ << var_name(v.index());
}

void SmtLibTermConverter::VisitCommonExpr(mp::Reference e) {
    assert(def_mapper_);
    out_ << def_mapper_->def(e.index());
}

void SmtLibTermConverter::VisitUnary(mp::UnaryExpr e) {
    VisitUnary(e, expr::str(e.kind()));
}

void SmtLibTermConverter::VisitMinus(mp::UnaryExpr e) {
    VisitUnary(e, "-");
}

void SmtLibTermConverter::VisitBinary(mp::BinaryExpr e) {
    std::string op = expr::str(e.kind());
    out_ << "(" << op << " ";
    Visit(e.lhs());
    out_ << " ";
    Visit(e.rhs());
    out_ << ")";
}

void SmtLibTermConverter::VisitIf(mp::IfExpr e) {
    out_ << "(ite ";
    Visit(e.condition());
    out_ << " ";
    Visit(e.then_expr());
    out_ << " ";
    Visit(e.else_expr());
    out_ << ")";
}

void SmtLibTermConverter::VisitPLTerm(mp::PLTerm e) {
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

void SmtLibTermConverter::VisitVarArg(VarArgExpr e) {
    std::string op = expr::str(e.kind());
    VisitNary(e, op, false);
}

void SmtLibTermConverter::VisitSum(SumExpr e) {
    VisitNary(e, "+", true);
}

void SmtLibTermConverter::VisitRelational(mp::RelationalExpr e) {
    std::string op = expr::str(e.kind());
    out_ << "(" << op << " ";
    Visit(e.lhs());
    out_ << " ";
    Visit(e.rhs());
    out_ << ")";
}

void SmtLibTermConverter::VisitUnary(mp::UnaryExpr e, const std::string &op) {
    out_ << "(" << op << " ";
    Visit(e.arg());
    out_ << ")";
}

template<typename T>
void SmtLibTermConverter::VisitNary(T &e, const std::string &op, bool can_squash) {
    if (e.num_args() == 0 && can_squash) {
        Visit(e.arg(0));
        return;
    }
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
        template_str = "{:f}";
    }

    if (d < 0) {
        return fmt::format("(- {})", fmt::format(template_str, -d));
    } else {
        return fmt::format(template_str, d);
    }
}

std::string var_name(int index) {
    return fmt::format("x{}", index);
}
//
//
//void
//objective_to_smtlib(const mp::BasicProblem<>::Objective &obj, mp::Problem &p, SmtLibTermConverter &converter) {
//    fmt::MemoryWriter &w = converter.writer();
//    std::string sense = obj.type() == mp::obj::MIN ? "minimize" : "maximize";
//    w << "(" << sense << "\n\t";
//    algebraic_expression_to_smtlib(obj, p, converter);
//    w << "\n)";
//}