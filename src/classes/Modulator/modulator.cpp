#include "modulator.h"

Modulator::Modulator(Waveformer& main, Waveformer& aux, LedRing& ring, algo_f_ptr* algo_arr):
    _main(main),
    _aux(aux),
    algo_ring(ring),
    ring_algos(algo_arr),
    core(21),
    xfade_len(50),
    xfade_mode(OFF)
{
    is_a = main.is_a;
}

void Modulator::generate() {
    uint32_t temp_acc = core.acc;
    if (!_main.running) {
        val = 0;
        xfade_mode == OFF;
    } else {
        incoming_idx = (is_a)? algo_ring.a_idx : algo_ring.b_idx;

        val = ring_algos[_idx](_main, _aux, *this) >> (fade_amt >> 2);

        if (incoming_idx != _idx && xfade_mode == OFF) {  // trigger xfade
            xfade_mode = XFADE_DOWN;
            fade_amt = 0;
        } else if (xfade_mode == XFADE_DOWN) {
            fade_amt++;
            if (fade_amt == xfade_len) {  // end of xfade_down
                xfade_mode == XFADE_UP;
                _idx = incoming_idx;  // switch to new algo
            }
        } else if (xfade_mode == XFADE_UP) {
            fade_amt--;
            if (fade_amt == 0) {  // end of xfade_up
                xfade_mode == OFF;
            }
        }
    }

    if (_main.mode == ENV) val = (val >> 1) + half_y;  // envelope scaling

    // not an algorithm that modifies the accumulator, keep it up to date for one that does
    if (temp_acc == core.acc) {
        core.reset(_main.core.acc);
    }
}