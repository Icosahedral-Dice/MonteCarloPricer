//
//  EuropeanOptionAnalyzer.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/24/22.
//

#include "EuropeanOptionAnalyzer.hpp"
#include "RNG.hpp"
#include "PathGenerator.hpp"
#include <cmath>
#include <numeric>

EuropeanOptionAnalyzer::EuropeanOptionAnalyzer(const EuropeanOption& option) : option_(option) {}

double EuropeanOptionAnalyzer::DiscountAndAverage(const std::vector<double>& vec) const {
    double res = std::accumulate(vec.cbegin(), vec.cend(), 0.);
    res /= vec.size();
    res *= std::exp(-option_.r_ * option_.T_);
    return res;
}

EuropeanOptionResults EuropeanOptionAnalyzer::Analyze(std::size_t N, unsigned long seed) const {
    
    // Reseed RNG machine
    LCE_uniform::reseed(seed);
    
    std::vector<double> Z(StandardGaussianMatrix::gen(N));
    std::vector<double> S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    std::vector<double> call;
    std::vector<double> delta_call;
    std::vector<double> vega_call;
    std::vector<double> put;
    std::vector<double> delta_put;
    std::vector<double> vega_put;
    
    for (std::size_t i = 0; i < Z.size(); i++) {
        if (S[i] > option_.K_) {
            call.push_back(S[i] - option_.K_);
            delta_call.push_back(S[i] / option_.S_);
            vega_call.push_back(S[i] * (-option_.sigma_ * option_.T_ + std::sqrt(option_.T_) * Z[i]));
            
            put.push_back(0.);
            delta_put.push_back(0.);
            vega_put.push_back(0.);
        } else {
            put.push_back(option_.K_ - S[i]);
            delta_put.push_back(-S[i] / option_.S_);
            vega_put.push_back(-S[i] * (-option_.sigma_ * option_.T_ + std::sqrt(option_.T_) * Z[i]));
            
            call.push_back(0.);
            delta_call.push_back(0.);
            vega_call.push_back(0.);
        }
    }
    
    return EuropeanOptionResults({this->DiscountAndAverage(call), this->DiscountAndAverage(delta_call), this->DiscountAndAverage(vega_call), this->DiscountAndAverage(put), this->DiscountAndAverage(delta_put), this->DiscountAndAverage(vega_put)});
    
}

double EuropeanOptionAnalyzer::Price(std::size_t N, const OptionType& type, const VarRed& modifier, unsigned long seed) const {
    
    // Reseed RNG machine
    LCE_uniform::reseed(seed);
    
    std::function<double (double)> payoff;
    
    switch (type) {
        case call:
            payoff = std::bind(option_.CallPayoff(), std::placeholders::_1, option_.T_);
            break;
        case put:
            payoff = std::bind(option_.PutPayoff(), std::placeholders::_1, option_.T_);
            break;
    }
    
    switch (modifier) {
        case vanilla:
            return PriceVanilla(N, payoff);
            break;
        case control_variate:
            return PriceCV(N, payoff);
            break;
        case antithetic_variables:
            return PriceAV(N, payoff);
            break;
        case moment_matching:
            return PriceMM(N, payoff);
            break;
        case MMCV:
            return PriceMMCV(N, payoff);
            break;
        default:
            return PriceVanilla(N, payoff);
            break;
    }
}

double EuropeanOptionAnalyzer::PriceVanilla(std::size_t N, const std::function<double (double)>& payoff) const {
    // Generate vector of standard Gaussian
    std::vector<double> Z(StandardGaussianMatrix::gen(N));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<double> S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff);
    
    return this->DiscountAndAverage(V);
}

std::vector<double> EuropeanOptionAnalyzer::PriceVanilla(std::size_t N, const std::function<double (double)>& payoff, const Dividend& proportional, const Dividend& fixed) const {
    
    std::vector<double> res;
    
    // !!!: HARDWIRED. FIX!!
    
    // Generate vector of standard Gaussian
    std::vector<std::vector<double>> Z(StandardGaussianMatrix::gen(N, 4));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<std::vector<double>> S_path(OneAssetWithPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z, proportional, fixed).S);
    
    std::vector<double> S(S_path.size());
    std::transform(S_path.cbegin(), S_path.cend(), S.begin(), [](const std::vector<double>& vec){ return vec.back(); });
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff);
    
    double value = this->DiscountAndAverage(V);
    
    res.push_back(value);
    
    // Get no-dividend S
    std::vector<double> S_nodiv(S.size());
    
    double t0 = 1./6., t1 = 1./6., t2 = 1./6., t3 = 1./12.;
    auto ZtoSNoDiv = [&](const std::vector<double>& z_path)->double {
        assert(z_path.size() == 4);
        return option_.S_ * std::exp((option_.r_ - option_.sigma_ * option_.sigma_ * .5) * option_.T_ + option_.sigma_ * (std::sqrt(t0) * z_path[0] + std::sqrt(t1) * z_path[1] + std::sqrt(t2) * z_path[2] + std::sqrt(t3) * z_path[3]));
    };
    
    std::transform(Z.cbegin(), Z.cend(), S_nodiv.begin(), ZtoSNoDiv);
    
    auto FindDelta = [&](double s, double real_s)->double {
        if (real_s < option_.K_) {
            return - s / option_.S_;
        } else {
            return 0.;
        }
    };
    
    std::vector<double> delta(S.size());
    std::transform(S_nodiv.cbegin(), S_nodiv.cend(), S.cbegin(), delta.begin(), FindDelta);
    double delta_hat = this->DiscountAndAverage(delta) * (1. - 0.02);
    res.push_back(delta_hat);
    
    // Control variates
    
    std::vector<double> V_nodiv(S.size());
    std::transform(S_nodiv.cbegin(), S_nodiv.cend(), V_nodiv.begin(), payoff);
    
    double sum_V = std::accumulate(V.cbegin(), V.cend(), 0.);
    double sum_V_nodiv = std::accumulate(V_nodiv.cbegin(), V_nodiv.cend(), 0.);
    double sum_V_V_nodiv = std::inner_product(V.cbegin(), V.cend(), V_nodiv.cbegin(), 0.);
    double sum_V_nodiv_squared = std::inner_product(V_nodiv.cbegin(), V_nodiv.cend(), V_nodiv.cbegin(), 0.);
    
    double b_hat = (sum_V_V_nodiv - sum_V * sum_V_nodiv / static_cast<double>(S.size())) / (sum_V_nodiv_squared - sum_V_nodiv * sum_V_nodiv / static_cast<double>(S.size()));
    
//    std::cout << "b: " << b_hat << std::endl;
//    std::cout << sum_V << '\t' << sum_V_nodiv << '\t' << sum_V_V_nodiv << '\t' << sum_V_nodiv_squared << std::endl;
    double W_hat = value - b_hat * (this->DiscountAndAverage(V_nodiv) - option_.Put());
    
    res.push_back(W_hat);
    
    std::vector<double> delta_nodiv(delta.size());
    std::transform(S_nodiv.cbegin(), S_nodiv.cend(), S_nodiv.cbegin(), delta_nodiv.begin(), FindDelta);
    
    double sum_delta = std::accumulate(delta.cbegin(), delta.cend(), 0.);
    double sum_delta_nodiv = std::accumulate(delta_nodiv.cbegin(), delta_nodiv.cend(), 0.);
    double sum_delta_delta_nodiv = std::inner_product(delta.cbegin(), delta.cend(), delta_nodiv.cbegin(), 0.);
    double sum_delta_nodiv_squared = std::inner_product(delta_nodiv.cbegin(), delta_nodiv.cend(), delta_nodiv.cbegin(), 0.);
    
    double b_delta_hat = (sum_delta_delta_nodiv - sum_delta * sum_delta_nodiv / static_cast<double>(S.size())) / (sum_delta_nodiv_squared - sum_delta_nodiv * sum_delta_nodiv / static_cast<double>(S.size()));
    
    double W_delta_hat = delta_hat - b_delta_hat * (this->DiscountAndAverage(delta_nodiv) - option_.DeltaPut());
    
    res.push_back(W_delta_hat);
    
    return res;
}

// Control variate
double EuropeanOptionAnalyzer::PriceCV(std::size_t N, const std::function<double (double)>& payoff) const {
    // Generate vector of standard Gaussian
    std::vector<double> Z(StandardGaussianMatrix::gen(N));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<double> S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff);
    
    // Discount payoff
    for (double& v : V) {
        v *= std::exp(-option_.r_ * option_.T_);
    }
        
    double sum_SV = std::inner_product(S.cbegin(), S.cend(), V.cbegin(), 0.);
    double sum_SS = std::inner_product(S.cbegin(), S.cend(), S.cbegin(), 0.);
    double sum_S = std::accumulate(S.cbegin(), S.cend(), 0.);
    double sum_V = std::accumulate(V.cbegin(), V.cend(), 0.);
    
    double b_hat = (sum_SV - sum_S * sum_V / static_cast<double>(S.size())) / (sum_SS - sum_S * sum_S / static_cast<double>(S.size()));
    
    double W_hat = sum_V / V.size() - b_hat * (sum_S / S.size() - std::exp(option_.r_ * option_.T_) * option_.S_);
    
    return W_hat;
}

// Antithetic variables
double EuropeanOptionAnalyzer::PriceAV(std::size_t N, const std::function<double (double)>& payoff) const {
    // Generate vector of standard Gaussian
    std::vector<double> Z(StandardGaussianMatrix::gen(N));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<double> S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    // Take the negative samples and generate more prices
    std::transform(Z.cbegin(), Z.cend(), Z.begin(), [](double z)->double { return -z; });
    std::vector<double> more_S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    S.insert(S.end(), more_S.cbegin(), more_S.cend());
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff);
    
    return this->DiscountAndAverage(V);
}

// Moment matching
double EuropeanOptionAnalyzer::PriceMM(std::size_t N, const std::function<double (double)>& payoff) const {
    
    // Generate vector of standard Gaussian
    std::vector<double> Z(StandardGaussianMatrix::gen(N));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<double> S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    // Adjust S to "match moments"
    double S_multiplier = option_.S_ * std::exp(option_.r_ * option_.T_) / std::accumulate(S.cbegin(), S.cend(), 0.) * S.size();
    std::transform(S.cbegin(), S.cend(), S.begin(), [=](double s)->double { return S_multiplier * s; });
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff);
    
    return this->DiscountAndAverage(V);
}

// Moment matching and control variables
double EuropeanOptionAnalyzer::PriceMMCV(std::size_t N, const std::function<double (double)>& payoff) const {
    // Generate vector of standard Gaussian
    std::vector<double> Z(StandardGaussianMatrix::gen(N));
    
    // From standard Gaussian get asset endpoint prices
    std::vector<double> S(OneAssetNoPath_BS(option_.S_, option_.T_, option_.sigma_, option_.r_, option_.q_, Z).S);
    
    // Adjust S to "match moments"
    double S_multiplier = option_.S_ * std::exp(option_.r_ * option_.T_) / std::accumulate(S.cbegin(), S.cend(), 0.) * S.size();
    std::transform(S.cbegin(), S.cend(), S.begin(), [=](double s)->double { return S_multiplier * s; });
    
    // Get payoff
    std::vector<double> V(S.size());
    std::transform(S.cbegin(), S.cend(), V.begin(), payoff);
    
    // Discount payoff
    for (double& v : V) {
        v *= std::exp(-option_.r_ * option_.T_);
    }
        
    double sum_SV = std::inner_product(S.cbegin(), S.cend(), V.cbegin(), 0.);
    double sum_SS = std::inner_product(S.cbegin(), S.cend(), S.cbegin(), 0.);
    double sum_S = std::accumulate(S.cbegin(), S.cend(), 0.);
    double sum_V = std::accumulate(V.cbegin(), V.cend(), 0.);
    
    double b_hat = (sum_SV - sum_S * sum_V / static_cast<double>(S.size())) / (sum_SS - sum_S * sum_S / static_cast<double>(S.size()));
    
    double W_hat = sum_V / V.size() - b_hat * (sum_S / S.size() - std::exp(option_.r_ * option_.T_) * option_.S_);
    
    return W_hat;
}
