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
    uint32_t temp_acc = acc;
    val = (_main.running)? ring_algos[(is_a)? algo_ring.a_idx : algo_ring.b_idx](_main, _aux, *this) : 0;
    if (_main.mode == ENV) val = (val >> 1) + half_y;

    // not an algorithm that modifies the accumulator, keep it up to date for one that does
    if (temp_acc == acc) {
        acc = _main.acc;
    }
}