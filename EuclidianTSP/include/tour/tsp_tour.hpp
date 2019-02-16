#pragma once

// reference: Data Structures for Travelling Salesmen
// Fredman, Johnson, McGeoch, Ostheimer

#include "tsp_util.hpp"


class Tour {
public:
    virtual Index Next(Index city) const = 0;
    virtual Index Prev(Index city) const = 0;
    virtual bool Between(Index a, Index b, Index c) const = 0;
    virtual void Flip(Index a, Index b, Index c, Index d) = 0;

    // Can implement WeakLineOrdered that gives 'true' on equal cities
    virtual bool LineOrdered(Index a, Index b) const = 0;

    // Reverse is order aware.
    // Resulting sequence may not preserve direction.
    virtual void Reverse(Index a, Index b) = 0;
};


