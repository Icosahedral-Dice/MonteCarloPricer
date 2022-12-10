//
//  PathDependentOption.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/27/22.
//

#include "PathDependentOption.hpp"
#include <numeric>
#include <iostream>

double PathDependentOption::Price(const std::vector<std::vector<double>>& S) const {
    std::vector<double> V;
    V.reserve(S.size());
    
    for (const std::vector<double>& path : S) {
        V.push_back(this->operator()(path));
    }
    
    return std::accumulate(V.cbegin(), V.cend(), 0.) / V.size();
}

BarrierOption::BarrierOption(const EuropeanOption& option, double B, const EuropeanOptionType& option_type, const BarrierType& barrier_type) : option_(option), B_(B), option_type_(option_type), barrier_type_(barrier_type) {}

EuropeanOption BarrierOption::GetVanillaOption() const {
    return option_;
}

double BarrierOption::operator () (const std::vector<double>& path) const {
    switch (barrier_type_) {
        case UpAndIn:
            {
                for (double node : path) {
                    if (node >= B_) {
                        switch (option_type_) {
                            case Call:
                                return option_.CallPayoff()(path.back(), option_.T_);
                            case Put:
                                return option_.PutPayoff()(path.back(), option_.T_);
                        }
                    }
                }
                return 0.;
            } break;
            
        case UpAndOut:
            {
                for (double node : path) {
                    if (node >= B_) {
                        return 0.;
                    }
                }
                switch (option_type_) {
                    case Call:
                        return option_.CallPayoff()(path.back(), option_.T_);
                    case Put:
                        return option_.PutPayoff()(path.back(), option_.T_);
                }
            } break;
            
        case DownAndIn:
            {
                for (double node : path) {
                    if (node <= B_) {
                        switch (option_type_) {
                            case Call:
                                return option_.CallPayoff()(path.back(), option_.T_);
                            case Put:
                                return option_.PutPayoff()(path.back(), option_.T_);
                        }
                    }
                }
                return 0.;
            } break;
            
        case DownAndOut:
            {
                for (double node : path) {
                    if (node <= B_) {
                        return 0.;
                    }
                }
                switch (option_type_) {
                    case Call:
                        return option_.CallPayoff()(path.back(), option_.T_);
                    case Put:
                        return option_.PutPayoff()(path.back(), option_.T_);
                }
            } break;
    }
}

double BarrierOption::BSPrice() const {
    switch (option_type_) {
        case Call:
            switch (barrier_type_) {
                case DownAndOut: {
                    double a = (option_.r_ - option_.q_) / (option_.sigma_ * option_.sigma_) - .5;
                    EuropeanOption option2(0., B_ * B_ / option_.S_, option_.K_, option_.T_, option_.sigma_, option_.r_, option_.q_);
                    double V = option_.Call() - std::pow((B_ / option_.S_), 2. * a) * option2.Call();
                    return V;
                }
                    
                case DownAndIn: {
                    double a = (option_.r_ - option_.q_) / (option_.sigma_ * option_.sigma_) - .5;
                    EuropeanOption option2(0., B_ * B_ / option_.S_, option_.K_, option_.T_, option_.sigma_, option_.r_, option_.q_);
                    double V = std::pow((B_ / option_.S_), 2. * a) * option2.Call();
                    return V;
                }
                    
                default:
                    return 0.;
            }
        case Put:
            return 0.;
    }
    
    
}
