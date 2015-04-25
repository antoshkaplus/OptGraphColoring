//
//  support.cpp
//  TravelingSalesman
//
//  Created by Anton Logunov on 7/4/13.
//  Copyright (c) 2013 Anton Logunov. All rights reserved.
//

#include "geometry.hpp"

Point operator-(const Point& p1, const Point& p2) {
    return Point(p1.x-p2.x, p1.y-p2.y);
}

void randomPoints(vector<Point>& ps, int number, long integerBound) {
    ps.resize(number);
    for (Point& p : ps) {
        p.x = random()%integerBound;
        p.y = random()%integerBound;
    }
}








