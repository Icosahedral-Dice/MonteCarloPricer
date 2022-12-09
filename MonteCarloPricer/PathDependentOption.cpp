//
//  PathDependentOption.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/27/22.
//

#include "PathDependentOption.hpp"
#include <numeric>

double PathDependentOption::Price(const std::vector<std::vector<double>>& S) const {
    std::vector<double> V;
    V.reserve(S.size());
    
    for (const std::vector<double>& path : S) {
        V.push_back(this->operator()(path));
    }
    
    return std::accumulate(V.cbegin(), V.cend(), 0.) / V.size();
}

double DownAndOutOption::operator () (const std::vector<double>& path) const {
    // TODO: Implement this
    return 0.;
}
