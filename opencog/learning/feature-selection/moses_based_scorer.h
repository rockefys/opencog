/** moses_based_scorer.h --- 
 *
 * Copyright (C) 2011 OpenCog Foundation
 *
 * Author: Nil Geisweiller <nilg@desktop>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef _OPENCOG_MOSES_BASED_SCORER_H
#define _OPENCOG_MOSES_BASED_SCORER_H

#include <opencog/util/numeric.h>

#include <opencog/learning/moses/eda/field_set.h>
#include <opencog/learning/moses/eda/eda.h>
#include <opencog/learning/moses/moses/types.h>

namespace opencog {

using namespace moses;

/**
 * translate an instance into a feature set.
 */
std::set<arity_t> get_feature_set(const eda::field_set& fields,
                                  const eda::instance& inst) {
    std::set<arity_t> fs;
    eda::field_set::const_bit_iterator bit = fields.begin_bits(inst);
    for(arity_t i = 0; bit != fields.end_bits(inst); bit++, i++)
        if(*bit)
            fs.insert(i);
    return fs;
}

/**
 * Wrapper to use a feature set scorer with MOSES's optimization
 * algorithms.
 */
template<typename FSScorer>
struct moses_based_scorer : public unary_function<eda::instance, composite_score> {

    moses_based_scorer(const FSScorer& fs_scorer, const eda::field_set& fields)
        : _fs_scorer(fs_scorer), _fields(fields) {}

    /**
     * The feature set is represented by an eda::instance encoding a
     * field of booleans. Each boolean represents whether its
     * corresponding feature is in the feature set of not.
     */
    composite_score operator()(const eda::instance& inst) const {
        std::set<arity_t> fs = get_feature_set(_fields, inst);
        composite_score csc(_fs_scorer(fs), fs.size());
        // Logger
        if(logger().getLevel() >= Logger::FINE) {
            stringstream ss;
            ss << "moses_based_scorer - Evaluate instance: " 
               << _fields.stream(inst) << " " << csc << std::endl;
            logger().fine(ss.str());
        }
        // ~Logger
        return csc;
    }

    const FSScorer& _fs_scorer;
    const eda::field_set& _fields;
};

} // ~namespace opencog

#endif // _OPENCOG_MOSES_BASED_SCORER_H