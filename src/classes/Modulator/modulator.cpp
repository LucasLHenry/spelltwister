#include "modulator.h"

Modulator::Modulator(Waveformer& main, Waveformer& aux, LedRing& ring, algo_f_ptr* algo_arr):
    _main(main),
    _aux(aux),
    algo_ring(ring),
    ring_algos(algo_arr),
    core(21),
    xfade_len(50),
    xfade_mode(OFF),
    xfade_amt(0)
{
    is_a = main.is_a;
    running = false;
}

void Modulator::generate() {
    uint32_t temp_acc = core.acc;
    if (!_main.running && !running) {
        val = 0;
    } else {
        incoming_idx = (is_a)? algo_ring.a_idx : algo_ring.b_idx;
        if (incoming_idx != _idx && xfade_mode == OFF) {
            xfade_mode = XFADE_DOWN;
            xfade_amt = 0;
        } else if (xfade_mode == XFADE_DOWN) {
            xfade_amt++;
            if (xfade_amt >= xfade_len) {
                xfade_mode = XFADE_UP;
                _idx = incoming_idx;
                param_a = 0;
                param_b = 0;
            }
        } else if (xfade_mode == XFADE_UP) {
            xfade_amt--;
            if (xfade_amt <= 0) {
                xfade_mode = OFF;
                xfade_amt = 0;
            }
        }
        int32_t centered_val = static_cast<int32_t>(ring_algos[_idx](_main, _aux, *this)) - half_y;
        val = static_cast<uint16_t>((centered_val >> (xfade_amt >> 2)) + half_y);
    }

    if (_main.mode == ENV) val = (val >> 1) + half_y;  // envelope scaling

    // not an algorithm that modifies the accumulator, keep it up to date for one that does
    if (temp_acc == core.acc) {
        core.reset(_main.core.acc);
    }
}