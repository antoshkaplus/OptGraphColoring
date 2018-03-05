//
//  support.h
//  TravelingSalesman
//
//  Created by Anton Logunov on 7/4/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#ifndef __TravelingSalesman__support__
#define __TravelingSalesman__support__

#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <array>

#include "ant/geometry/d2.h"

using namespace std;
using namespace ant;
using namespace ant::geometry;
using namespace ant::geometry::d2;

struct Point {
    double x, y;
    double distance(const Point& p) const {
        return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y));
    }
    Point() {}
    Point(double x, double y) : x(x), y(y) {}
    Point& operator=(const Point& p) {
        x = p.x;
        y = p.y;
        return *this;
    }
    
    static double crossProduct(const Point& p1, const Point& p2) {
        return p1.x*p2.y - p1.y*p2.x;
    }
};

Point operator-(const Point& p1, const Point& p2);


#endif /* defined(__TravelingSalesman__support__) */


























