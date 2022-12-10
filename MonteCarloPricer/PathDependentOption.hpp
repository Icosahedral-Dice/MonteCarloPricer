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
public:
    PathDependentOption() = default;
    ~PathDependentOption() = default;
    
    virtual double operator () (const std::vector<double>& path) const = 0;
    double Price(const std::vector<std::vector<double>>& S) const;
};

enum BarrierType {
    UpAndIn,
    UpAndOut,
    DownAndIn,
    DownAndOut,
};

class BarrierOption : public PathDependentOption {
private:
    EuropeanOption option_; // Corresponding European option
    double B_;
    bool is_put_;
    EuropeanOptionType option_type_;
    BarrierType barrier_type_;
    
public:
    BarrierOption(const EuropeanOption& option, double B, const EuropeanOptionType& option_type, const BarrierType& barrier_type);
    
    EuropeanOption GetVanillaOption() const;
    
    virtual double operator () (const std::vector<double>& path) const override;
    
    // Theoretical price
    double BSPrice() const;
};

#endif /* PathDependentOption_hpp */
