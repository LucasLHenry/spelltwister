#include "modulator.h"

Modulator::Modulator(Waveformer& main, Waveformer& aux, LedRing& ring, algo_f_ptr* algo_arr):
    _main(main),
    _aux(aux),
    algo_ring(ring),
    ring_algos(algo_arr),
    core(21)
{
    is_a = main.is_a;
}

void Modulator::generate() {
    uint32_t temp_acc = core.acc;
    if (!_main.running) {
        val = 0;
    } else {
        uint16_t idx = (is_a)? algo_ring.a_idx : algo_ring.b_idx;
        val = ring_algos[idx](_main, _aux, *this);
    }

    if (_main.mode == ENV) val = (val >> 1) + half_y;  // envelope scaling

    // not an algorithm that modifies the accumulator, keep it up to date for one that does
    if (temp_acc == core.acc) {
        core.reset(_main.core.acc);
    }
}