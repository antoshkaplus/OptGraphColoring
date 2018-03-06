//
//  MFL_MIP.h
//  FacilityLocation
//
//  Created by Anton Logunov on 4/28/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FacilityLocation__MFL_MIP__
#define __FacilityLocation__MFL_MIP__

#include <iostream>

#include "glpk.h"

#include "MFL.h"

struct MFL_MIP : MFL_Solver {
    
    std::vector<FacilityIndex> solve(const std::vector<Customer>& customers, 
                                     const std::vector<Facility>& facilities) {
        glp_prob *mip = glp_create_prob();
        glp_set_obj_dir(mip, GLP_MIN);
        
        // sum{j} (x_ij) = 1 // customer has only one assigned facility
        Count eq_constr_count = customers.size();
        Index eq_constr_begin = glp_add_rows(mip, eq_constr_count);
        for (Index i = eq_constr_begin; i < eq_constr_begin+eq_constr_count; ++i) {
            glp_set_row_bnds(mip, i, GLP_FX, 1, 1);
        }
        // x_ij - y_j <= 0 // customer can't go to closed facility
        Count ls_constr_count = customers.size()*facilities.size();
        Index ls_constr_begin = glp_add_rows(mip, ls_constr_count);
        for (Index i = ls_constr_begin; i < ls_constr_begin+ls_constr_count; ++i) {
            glp_set_row_bnds(mip, i, GLP_UP, 0, 0);
        }
        
        Count cap_constr_count = facilities.size();
        Index cap_constr_begin = glp_add_rows(mip, cap_constr_count);
        for (Index i = cap_constr_begin; i < cap_constr_begin+cap_constr_count; ++i) {
            glp_set_row_bnds(mip, i, GLP_UP, facilities[i-cap_constr_begin].capacity, facilities[i-cap_constr_begin].capacity);
        }
        
        // is customer goes to facility // is facility opened
        Count col_count = customers.size()*facilities.size() + facilities.size();
        Index col_begin = glp_add_cols(mip, col_count);
        for (Index i = col_begin; i < col_begin + col_count; ++i) {
            glp_set_col_kind(mip, i, GLP_BV);
        }
        // looping through customers
        for (Index i = 0; i < customers.size(); ++i) {
            // looping through facilities
            for (Index j = 0; j < facilities.size(); ++j) {
                glp_set_obj_coef(mip, i*facilities.size() + j + 1, distance(customers[i], facilities[j]));
            }
        }
        Count i_begin = customers.size()*facilities.size()+1; 
        for (Index i = 0; i < facilities.size(); ++i) {
            glp_set_obj_coef(mip, i_begin + i, facilities[i].cost);
        }
        
        // adding eq_constr coeff for each customer
        vector<int> ka(facilities.size());
        vector<double> ar(facilities.size(), 1.);
        for (Index i = eq_constr_begin; i < eq_constr_begin+eq_constr_count; ++i) {
            iota(ka.begin(), ka.end(), (i-1)*facilities.size()+1);
            glp_set_mat_row(mip, i, facilities.size(), ka.data()-1, ar.data()-1);
        }
        // need 1-s for every x_ij (i changes), j fixed. -1 on j facility 
        ka.resize(customers.size()+1);
        ar.resize(customers.size()+1, 1.);
        ar.back() = -1.;
        
        
        cout << col_count << endl;
        
        for (Index i = 0; i < customers.size(); ++i) {
            for (Index j = 0; j < facilities.size(); ++j) {
                int ka[2+1] = {0, 
                                static_cast<int>(i*facilities.size() + j + 1), 
                                static_cast<int>(customers.size()*facilities.size() + 1 + j) };
                glp_set_mat_row(
                    mip, 
                    ls_constr_begin + i*facilities.size() + j, 
                    2, 
                    ka,
//                    (int[2]){
//                        static_cast<int>(i*facilities.size() + j + 1), 
//                        static_cast<int>(customers.size()*facilities.size() + 1 + j)},
                    (const double[2+1]){0, 1., -1.});
            }
        }
        
        ka.resize(customers.size()+1);
        ar.resize(customers.size()+1);
        for (Index i = 1; i <= customers.size(); ++i) {
            ar[i] = customers[i-1].demand;
        }
        for (Index j = 0; j < facilities.size(); ++j) {

            for (auto k = 1; k <= customers.size(); ++k) {
                ka[k] = j*facilities.size() + 1;
            }
            glp_set_mat_row(
                mip, 
                j+cap_constr_begin, 
                customers.size(), 
                ka.data(), 
                ar.data());
        }
        
        glp_write_lp(mip, nullptr, "pr.txt");
        
        // this should work somehow
        glp_iocp parm;
        glp_init_iocp(&parm);
        parm.presolve = GLP_ON;
        
        int err = glp_intopt(mip, &parm);
        
        vector<FacilityIndex> assignment(customers.size());
        for (Index i = 0; i < customers.size(); ++i) {
            for (Index j = 0; j < facilities.size(); ++j) {
                if (glp_mip_col_val(mip, i*facilities.size() + j + 1) == 1) {
                    assignment[i] = j;
                    break;
                }
            }
        }
        
        glp_delete_prob(mip);
        
        return assignment;
    }
    
};



#endif /* defined(__FacilityLocation__MFL_MIP__) */
