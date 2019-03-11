#pragma once

struct LK_Interface {

    virtual vector<City> Solve() = 0;

    virtual int64_t iter_count() const = 0;
    virtual const vector<Count>& close_count() const = 0;
    virtual bool time_limit_reached() const = 0;
    virtual Duration time_spent() const = 0;

    virtual ~LK_Interface() {}
};