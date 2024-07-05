#include "modulator.h"

Modulator::Modulator(Waveformer& main, Waveformer& aux, LedRing& ring, algo_f_ptr* algo_arr):
    _main(main),
    _aux(aux),
    algo_ring(ring),
    ring_algos(algo_arr)
{
    is_a = main.is_a;
}

void Modulator::generate() {
    val = (_main.running)? ring_algos[(is_a)? algo_ring.a_idx : algo_ring.b_idx](_main, _aux, *this) : 0;
    if (_main.mode == ENV) val = (val >> 1) + half_y;
}