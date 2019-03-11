#pragma once

// y : returns another endpoint of added t_i
inline Index added_endpoint(Index t_index, Count t_sz) {
    return (t_index % 2 == 0) ? (t_index + t_sz - 1) % t_sz : (t_index+1) % t_sz;
}

// x : returns another endpoint of broken t_i
inline Index broken_endpoint(Index t_index, Count t_sz) {
    return (t_index % 2 == 0) ? (t_index + t_sz + 1) % t_sz :  t_index-1;
}

template <typename Tour, typename Permutation>
void Close(Tour& tour, const Permutation& perm) {

    auto i_1 = perm.city(0);
    for (auto i = 1; i < perm.size()-2; i+=2) {
        auto i_2 = perm.city(i);
        auto i_3 = perm.city(i+1);
        auto i_4 = perm.city(i+2);
        tour.Flip(i_1, i_2, i_3, i_4);
    }

}