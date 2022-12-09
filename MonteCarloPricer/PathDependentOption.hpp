//
//  PathDependentOption.hpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/27/22.
//

#ifndef PathDependentOption_hpp
#define PathDependentOption_hpp

#include <functional>
#include <vector>
#include "EuropeanOption.hpp"

class PathDependentOption {
private:
    PathDependentOption() = default;
    ~PathDependentOption() = default;
    
public:
    virtual double operator () (const std::vector<double>& path) const = 0;
    double Price(const std::vector<std::vector<double>>& S) const;
};

class DownAndOutOption : public PathDependentOption {
private:
    EuropeanOption option_; // Corresponding European option
    double B_;
    
public:
    DownAndOutOption(const EuropeanOption& option, double B);
    // TODO: Implement this
    virtual double operator () (const std::vector<double>& path) const override;
};

#endif /* PathDependentOption_hpp */
