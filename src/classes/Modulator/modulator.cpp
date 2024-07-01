#include "modulator.h"

Modulator::Modulator(Waveformer& A, Waveformer& B, LedRing& ring, algo_f_ptr* algo_arr):
    a(A),
    b(B),
    algo_ring(ring),
    ring_algos(algo_arr)
{}

void Modulator::generate_a() {
    a_val = (a.running)? ring_algos[algo_ring.a_idx](a, b) : 0;
    if (a.mode == ENV) a_val = (a_val >> 1) + half_y;
}

void Modulator::generate_b() {
    b_val = (b.running)? ring_algos[algo_ring.b_idx](b, a) : 0;
    if (b.mode == ENV) b_val = (b_val >> 1) + half_y;
}