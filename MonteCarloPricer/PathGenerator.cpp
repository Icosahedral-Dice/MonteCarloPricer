//
//  PathGenerator.cpp
//  MonteCarloPricer
//
//  Created by 王明森 on 11/24/22.
//

#include "PathGenerator.hpp"
#include <algorithm>
#include <cmath>

OneAssetNoPath::OneAssetNoPath(std::size_t size) : S(size) {}

OneAssetNoPath_BS::OneAssetNoPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<double>& z_arr) : OneAssetNoPath(z_arr.size()) {
    
    auto z_to_s = [&](double z)->double {
        return S0 * std::exp((r - q - sigma * sigma / 2.) * T + sigma * std::sqrt(T) * z);
    };
    
    std::transform(z_arr.cbegin(), z_arr.cend(), S.begin(), z_to_s);
}

OneAssetWithPath::OneAssetWithPath(const std::vector<std::vector<double>>& z_arr) : S(z_arr) {}

OneAssetWithPath_BS::OneAssetWithPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<std::vector<double>>& z_arr) : OneAssetWithPath(z_arr) {
    
    double dt = T / z_arr[0].size();
    
    for (std::vector<double>& path : S) {
        double curr_S = S0;
        for (double& node: path) {
            curr_S *= std::exp((r - q - sigma * sigma / 2.) * dt + sigma * std::sqrt(dt) * node);
            node = curr_S;
        }
    }
}

OneAssetWithPath_BS::OneAssetWithPath_BS(double S0, double T, double sigma, double r, double q, const std::vector<std::vector<double>>& z_arr, const Dividend& proportional, const Dividend& fixed) : OneAssetWithPath(z_arr) {
    
    auto tit_prop = proportional.dates.cbegin();
    auto dit_prop = proportional.dividends.cbegin();
    auto tit_fixed = fixed.dates.cbegin();
    auto dit_fixed = fixed.dividends.cbegin();
    
    std::vector<double> dividend_value;
    std::vector<bool> dividend_is_fixed;
    std::vector<double> time_diff;
    
    double curr_time = 0.;
    while ((tit_prop != proportional.dates.cend()) || (tit_fixed != fixed.dates.cend())) {
        if (tit_prop == proportional.dates.cend()) {
            // Proportional dividends exhausted, apply all remaining fixed dividends
            while (tit_fixed != fixed.dates.cend()) {
                dividend_value.push_back(*dit_fixed);
                dividend_is_fixed.push_back(true);
                
                time_diff.push_back(*tit_fixed - curr_time);
                curr_time = *tit_fixed;
                
                dit_fixed++;
                tit_fixed++;
            }
            break;
        } else if (tit_fixed == fixed.dates.cend()) {
            // Fixed dividends exhausted, apply all remaining proportional dividends
            while (tit_prop != proportional.dates.cend()) {
                dividend_value.push_back(*dit_prop);
                dividend_is_fixed.push_back(false);
                
                time_diff.push_back(*tit_prop - curr_time);
                curr_time = *tit_prop;
                
                dit_prop++;
                tit_prop++;
            }
            break;
        } else {
            // Both types of dividends expected. Apply the earliest dividend possible.
            // Proportional dividends go first if (in the unlikely case that) two dividends coincide in time
            if ((*tit_fixed) < (*tit_prop)) {
                // The fixed dividend is earlier
                dividend_value.push_back(*dit_fixed);
                dividend_is_fixed.push_back(true);
                
                time_diff.push_back(*tit_fixed - curr_time);
                curr_time = *tit_fixed;
                
                dit_fixed++;
                tit_fixed++;
            } else {
                // The proportional dividend is earlier
                dividend_value.push_back(*dit_prop);
                dividend_is_fixed.push_back(false);
                
                time_diff.push_back(*tit_prop - curr_time);
                curr_time = *tit_prop;
                
                dit_prop++;
                tit_prop++;
            }
        }
    }
    
    time_diff.push_back(T - curr_time);
    
    for (std::vector<double>& path : S) {
        double curr_S = S0;
        for (std::size_t i = 0; i < path.size(); i++) {
            curr_S *= std::exp((r - sigma * sigma / 2.) * time_diff[i] + sigma * std::sqrt(time_diff[i]) * path[i]);
            if (i + 1 == path.size()) {
                
            } else if (dividend_is_fixed[i]) {
                curr_S -= dividend_value[i];
            } else {
                curr_S *= (1. - dividend_value[i]);
            }
            path[i] = curr_S;
        }
    }
}
