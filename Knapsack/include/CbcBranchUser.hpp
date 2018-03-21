#pragma once

#include <exception>

#include "coin/CbcBranchBase.hpp"
#include "coin/CbcBranchActual.hpp"


class CbcBranchUserDecision : public CbcBranchDecision {
public:
    // Default Constructor
    CbcBranchUserDecision() {}

    // Copy constructor
    CbcBranchUserDecision(const CbcBranchUserDecision& rhs) : CbcBranchDecision(rhs) {}

    virtual ~CbcBranchUserDecision() {}

    virtual CbcBranchDecision *clone() const {
        return new CbcBranchUserDecision(*this);
    }

    virtual void initialize(CbcModel *model) {

    }

    virtual int betterBranch(CbcBranchingObject *thisOne,
                             CbcBranchingObject *bestSoFar,
                             double changeUp, int numberInfeasibilitiesUp,
                             double changeDown, int numberInfeasibilitiesDown) {

        throw std::runtime_error("Now obsolete CbcBranchUserDecision::betterBranch");
    }

    /** \brief Compare N branching objects. Return index of best
        and sets way of branching in chosen object.

      This routine is used only after strong branching.
      This is reccommended version as it can be more sophisticated
    */

    virtual int bestBranch(CbcBranchingObject **objects, int numberObjects, int numberUnsatisfied,
                           double *changeUp, int *numberInfeasibilitiesUp,
                           double *changeDown, int *numberInfeasibilitiesDown,
                           double objectiveValue) {


        int bestWay=0;
        int whichObject = -1;
        if (numberObjects) {
            CbcModel * model = objects[0]->model();
            // at continuous
            //double continuousObjective = model->getContinuousObjective();
            //int continuousInfeasibilities = model->getContinuousInfeasibilities();

            // average cost to get rid of infeasibility
            //double averageCostPerInfeasibility =
            //(objectiveValue-continuousObjective)/
            //(double) (abs(continuousInfeasibilities-numberUnsatisfied)+1);
            /* beforeSolution is :
               0 - before any solution
               n - n heuristic solutions but no branched one
               -1 - branched solution found
            */
            int numberSolutions = model->getSolutionCount();
            double cutoff = model->getCutoff();
            int method=0;
            int i;
            if (numberSolutions) {
                int numberHeuristic = model->getNumberHeuristicSolutions();
                if (numberHeuristic<numberSolutions) {
                    method = 1;
                } else {
                    method = 2;
                    // look further
                    for ( i = 0 ; i < numberObjects ; i++) {
                        int numberNext = numberInfeasibilitiesUp[i];

                        if (numberNext<numberUnsatisfied) {
                            int numberUp = numberUnsatisfied - numberInfeasibilitiesUp[i];
                            double perUnsatisfied = changeUp[i]/(double) numberUp;
                            double estimatedObjective = objectiveValue + numberUnsatisfied * perUnsatisfied;
                            if (estimatedObjective<cutoff)
                                method=3;
                        }
                        numberNext = numberInfeasibilitiesDown[i];
                        if (numberNext<numberUnsatisfied) {
                            int numberDown = numberUnsatisfied - numberInfeasibilitiesDown[i];
                            double perUnsatisfied = changeDown[i]/(double) numberDown;
                            double estimatedObjective = objectiveValue + numberUnsatisfied * perUnsatisfied;
                            if (estimatedObjective<cutoff)
                                method=3;
                        }
                    }
                }
                method=2;
            } else {
                method = 0;
            }
            // Uncomment next to force method 4
            //method=4;
            /* Methods :
               0 - fewest infeasibilities
               1 - largest min change in objective
               2 - as 1 but use sum of changes if min close
               3 - predicted best solution
               4 - take cheapest up branch if infeasibilities same
            */
            int bestNumber=COIN_INT_MAX;
            double bestCriterion=-1.0e50;
            double alternativeCriterion = -1.0;
            double bestEstimate = 1.0e100;
            switch (method) {
                case 0:
                    // could add in depth as well
                    for ( i = 0 ; i < numberObjects ; i++) {
                        int thisNumber = CoinMin(numberInfeasibilitiesUp[i],numberInfeasibilitiesDown[i]);
                        if (thisNumber<=bestNumber) {
                            int betterWay=0;
                            if (numberInfeasibilitiesUp[i]<numberInfeasibilitiesDown[i]) {
                                if (numberInfeasibilitiesUp[i]<bestNumber) {
                                    betterWay = 1;
                                } else {
                                    if (changeUp[i]<bestCriterion)
                                        betterWay=1;
                                }
                            } else if (numberInfeasibilitiesUp[i]>numberInfeasibilitiesDown[i]) {
                                if (numberInfeasibilitiesDown[i]<bestNumber) {
                                    betterWay = -1;
                                } else {
                                    if (changeDown[i]<bestCriterion)
                                        betterWay=-1;
                                }
                            } else {
                                // up and down have same number
                                bool better=false;
                                if (numberInfeasibilitiesUp[i]<bestNumber) {
                                    better=true;
                                } else if (numberInfeasibilitiesUp[i]==bestNumber) {
                                    if (CoinMin(changeUp[i],changeDown[i])<bestCriterion)
                                        better=true;;
                                }
                                if (better) {
                                    // see which way
                                    if (changeUp[i]<=changeDown[i])
                                        betterWay=1;
                                    else
                                        betterWay=-1;
                                }
                            }
                            if (betterWay) {
                                bestCriterion = CoinMin(changeUp[i],changeDown[i]);
                                bestNumber = thisNumber;
                                whichObject = i;
                                bestWay = betterWay;
                            }
                        }
                    }
                    break;
                case 1:
                    for ( i = 0 ; i < numberObjects ; i++) {
                        int betterWay=0;
                        if (changeUp[i]<=changeDown[i]) {
                            if (changeUp[i]>bestCriterion)
                                betterWay=1;
                        } else {
                            if (changeDown[i]>bestCriterion)
                                betterWay=-1;
                        }
                        if (betterWay) {
                            bestCriterion = CoinMin(changeUp[i],changeDown[i]);
                            whichObject = i;
                            bestWay = betterWay;
                        }
                    }
                    break;
                case 2:
                    for ( i = 0 ; i < numberObjects ; i++) {
                        double change = CoinMin(changeUp[i],changeDown[i]);
                        double sum = changeUp[i] + changeDown[i];
                        bool take=false;
                        if (change>1.1*bestCriterion)
                            take=true;
                        else if (change>0.9*bestCriterion&&sum+change>bestCriterion+alternativeCriterion)
                            take=true;
                        if (take) {
                            if (changeUp[i]<=changeDown[i]) {
                                if (changeUp[i]>bestCriterion)
                                    bestWay=1;
                            } else {
                                if (changeDown[i]>bestCriterion)
                                    bestWay=-1;
                            }
                            bestCriterion = change;
                            alternativeCriterion = sum;
                            whichObject = i;
                        }
                    }
                    break;
                case 3:
                    for ( i = 0 ; i < numberObjects ; i++) {
                        int numberNext = numberInfeasibilitiesUp[i];

                        if (numberNext<numberUnsatisfied) {
                            int numberUp = numberUnsatisfied - numberInfeasibilitiesUp[i];
                            double perUnsatisfied = changeUp[i]/(double) numberUp;
                            double estimatedObjective = objectiveValue + numberUnsatisfied * perUnsatisfied;
                            if (estimatedObjective<bestEstimate) {
                                bestEstimate = estimatedObjective;
                                bestWay=1;
                                whichObject=i;
                            }
                        }
                        numberNext = numberInfeasibilitiesDown[i];
                        if (numberNext<numberUnsatisfied) {
                            int numberDown = numberUnsatisfied - numberInfeasibilitiesDown[i];
                            double perUnsatisfied = changeDown[i]/(double) numberDown;
                            double estimatedObjective = objectiveValue + numberUnsatisfied * perUnsatisfied;
                            if (estimatedObjective<bestEstimate) {
                                bestEstimate = estimatedObjective;
                                bestWay=-1;
                                whichObject=i;
                            }
                        }
                    }
                    break;
                case 4:
                    // if number infeas same then cheapest up
                    // first get best number or when going down
                    // now choose smallest change up amongst equal number infeas
                    for ( i = 0 ; i < numberObjects ; i++) {
                        int thisNumber = CoinMin(numberInfeasibilitiesUp[i],numberInfeasibilitiesDown[i]);
                        if (thisNumber<=bestNumber) {
                            int betterWay=0;
                            if (numberInfeasibilitiesUp[i]<numberInfeasibilitiesDown[i]) {
                                if (numberInfeasibilitiesUp[i]<bestNumber) {
                                    betterWay = 1;
                                } else {
                                    if (changeUp[i]<bestCriterion)
                                        betterWay=1;
                                }
                            } else if (numberInfeasibilitiesUp[i]>numberInfeasibilitiesDown[i]) {
                                if (numberInfeasibilitiesDown[i]<bestNumber) {
                                    betterWay = -1;
                                } else {
                                    if (changeDown[i]<bestCriterion)
                                        betterWay=-1;
                                }
                            } else {
                                // up and down have same number
                                bool better=false;
                                if (numberInfeasibilitiesUp[i]<bestNumber) {
                                    better=true;
                                } else if (numberInfeasibilitiesUp[i]==bestNumber) {
                                    if (CoinMin(changeUp[i],changeDown[i])<bestCriterion)
                                        better=true;;
                                }
                                if (better) {
                                    // see which way
                                    if (changeUp[i]<=changeDown[i])
                                        betterWay=1;
                                    else
                                        betterWay=-1;
                                }
                            }
                            if (betterWay) {
                                bestCriterion = CoinMin(changeUp[i],changeDown[i]);
                                bestNumber = thisNumber;
                                whichObject = i;
                                bestWay = betterWay;
                            }
                        }
                    }
                    bestCriterion=1.0e50;
                    for ( i = 0 ; i < numberObjects ; i++) {
                        int thisNumber = numberInfeasibilitiesUp[i];
                        if (thisNumber==bestNumber&&changeUp) {
                            if (changeUp[i]<bestCriterion) {
                                bestCriterion = changeUp[i];
                                whichObject = i;
                                bestWay = 1;
                            }
                        }
                    }
                    break;
            }
            // set way in best
            if (whichObject>=0)
                objects[whichObject]->way(bestWay);
        }
        return whichObject;
    }

private:

    CbcBranchUserDecision& operator=(const CbcBranchUserDecision& rhs);
};
