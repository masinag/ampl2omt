//
// Created by gabriele on 29/09/24.
//

#include "ampl2omt/definitions_mapper.hpp"

// ----------------------------
// DefinitionsGetter
// ----------------------------

void DefinitionsGetter::VisitCommonExpr(mp::Reference e) {
    if (has_definition_[e.index()]) {
        return;
    }
    has_definition_[e.index()] = true;
    Visit(p_->common_expr(e.index()).nonlinear_expr());
}

void DefinitionsGetter::reset_definitions() {
    has_definition_.assign(p_->num_common_exprs(), false);
}

void DefinitionsGetter::set_problem(const mp::Problem &p) {
    p_ = &p;
}


// ----------------------------
// DefinitionsMapper
// ----------------------------

std::vector<int> DefinitionsMapper::defs() const {
    return defs_;
}

std::string DefinitionsMapper::def(int i) const {
    assert(i < defs_.size());
    return fmt::format(".def_{}{}", prefix_, i);
}

