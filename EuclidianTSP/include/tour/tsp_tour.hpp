#pragma once

// reference: Data Structures for Travelling Salesmen
// Fredman, Johnson, McGeoch, Ostheimer

#include "tsp_util.hpp"


class Tour {
public:
    virtual Index Next(Index city) = 0;
    virtual Index Prev(Index city) = 0;
    virtual bool Between(Index a, Index b, Index c) = 0;
    // TODO consider not implementing Flip and have something different
    virtual void Flip(Index a, Index b, Index c, Index d) = 0;
    virtual void Reverse(Index a, Index b) = 0;
};


