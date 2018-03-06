//
//  MFL_Mixed.h
//  FacilityLocation
//
//  Created by Anton Logunov on 4/29/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __FacilityLocation__MFL_Mixed__
#define __FacilityLocation__MFL_Mixed__

#include <iostream>

#include <boost/range.hpp>

#include "glpk.h"

#include "MFL.h"

vector<double> heuristics;
  
void callback(glp_tree *tree, void *info) {
    if (glp_ios_reason(tree) == GLP_IHEUR) {
        glp_ios_heur_sol(tree, heuristics.data());
    }
}

struct MFL_Mixed : MFL_Improver {

    std::vector<FacilityIndex> improve(
        const std::vector<Customer>& customers, 
        const std::vector<Facility>& facilities,
        const std::vector<FacilityIndex>& customerFacilities) override {
        
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
        
        // capacity constraints
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
        Count col_continue = customers.size()*facilities.size()+1; 
        for (Index j = 0; j < facilities.size(); ++j) {
            glp_set_obj_coef(mip, col_continue + j, facilities[j].cost);
        }
        
        // adding eq_constr coeff for each customer
        vector<int> ka(facilities.size()+1);
        vector<double> ar(facilities.size()+1, 1.);
        for (Index i = eq_constr_begin; i < eq_constr_begin+eq_constr_count; ++i) {
            iota(ka.begin()+1, ka.end(), (i-1)*facilities.size()+1);
            glp_set_mat_row(mip, i, facilities.size(), ka.data(), ar.data());
        }
        // need 1-s for every x_ij (i changes), j fixed. -1 on j facility 
        ka.resize(2+1);
        ar.resize(2+1, 1.);
        ar.back() = -1.;
        Index index = ls_constr_begin;
        for (Index i = 0; i < customers.size(); ++i) {
            for (Index j = 0; j < facilities.size(); ++j, ++index) {
                ka[1] = i*facilities.size() + j + 1;
                ka[2] = customers.size()*facilities.size() + 1 + j;
                glp_set_mat_row(mip, index, 2, 
                                ka.data(),
                                ar.data());
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
            glp_set_mat_row(mip, 
                            j+cap_constr_begin, 
                            customers.size(), 
                            ka.data(), 
                            ar.data());
        }
        
        // this should work somehow
        glp_iocp parm;
        glp_init_iocp(&parm);
        parm.presolve = GLP_ON;
        parm.br_tech = GLP_BR_MFV;
        parm.gmi_cuts = GLP_ON;
        
        heuristics.resize(col_count, 0);
        vector<bool> is_facility_used(facilities.size(), false);
        for (auto i = 0; i < customers.size(); ++i) {
            heuristics[i*facilities.size()+1+customerFacilities[i]] = 1;
            is_facility_used[customerFacilities[i]] = true;
        }
        
        for (auto j = 0; j < facilities.size(); ++j) {
            heuristics[col_continue+j] = is_facility_used[j];
        }
        
        parm.cb_func = callback;        
        
        
        int err = glp_intopt(mip, &parm);
        
        cout << "lol" << endl;
        
        vector<FacilityIndex> assignment(customers.size());
        for (Index i = 0; i < customers.size(); ++i) {
            for (Index j = 0; j < facilities.size(); ++j) {
                if (glp_mip_col_val(mip, i*facilities.size() + j + 1) == 1) {
                    assignment[i] = j;
                    break;
                }
            }
        }
        
        glp_write_mip(mip, "sol.txt");
        
        glp_delete_prob(mip);
        
        return assignment;

                                               
    }
};



#endif /* defined(__FacilityLocation__MFL_Mixed__) */
