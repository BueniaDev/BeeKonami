// Usage (to generate 8-bit colors):
// g++ gen_csv.cpp --std=c++17 -O3 -o gen_csv && ./gen_csv > k052535_input.csv
// ./nltool -c run -i k052535_input.csv -l OUT -t 0.0013 nl_k052535.cpp
// ./nlwav -f tab -o values.tab -s 0.0010005 -i 0.000008 -n 32 log_OUT.log
// g++ convert_dac.cpp --std=c++17 -O3 -o convert_dac && ./convert_dac
// (Final color values will be in the "values_out.bin" file

#include "netlist/devices/net_lib.h"

NETLIST_START(k052535_test)
{
    PARAM(Solver.RELTOL, 1e-2)
    PARAM(Solver.VNTOL, 1e-6)
    PARAM(Solver.NR_LOOPS, 30)
    PARAM(Solver.GS_LOOPS, 99)
    PARAM(Solver.METHOD, "MAT_CR")
    PARAM(Solver.PARALLEL, 0)

    SOLVER(Solver, 48000)
    PARAM(Solver.DYNAMIC_TS, 1)
    PARAM(Solver.DYNAMIC_MIN_TIMESTEP, 2e-8)
    PARAM(Solver.DYNAMIC_LTE, 1e-4)

    ANALOG_INPUT(V5, 5)
    ALIAS(VCC, V5)

    TTL_INPUT(input_blank, 1)

    LOGIC_INPUT8(input, 0, "74XX")
    TTL_7407_DIP(U44)

    NET_C(VCC, input.VCC, U44.14, input_blank.VCC)
    NET_C(GND, input.GND, U44.7, input_blank.GND)

    NET_C(input.Q0, U44.1)
    NET_C(input.Q1, U44.3)
    NET_C(input.Q2, U44.5)
    NET_C(input.Q3, U44.9)
    NET_C(input.Q4, U44.11)
    NET_C(input_blank, U44.13)

    ALIAS(D0, U44.2)
    ALIAS(D1, U44.4)
    ALIAS(D2, U44.6)
    ALIAS(D3, U44.8)
    ALIAS(D4, U44.10)
    ALIAS(BLANK, U44.12)

    INCLUDE(k052535_dac)

    // Uncomment the below section of code to enable the shadow logic used in TMNT
    /*
    TTL_7407_DIP(U45)
    NET_C(VCC, U45.14)
    NET_C(GND, U45.7)
    NET_C(GND, U45.1)
    NET_C(GND, U45.3, U45.5, U45.9, U45.11, U45.13)
    ALIAS(SHAD, U45.2)

    RES(R38, 510)
    NET_C(SHAD, R38.2)
    NET_C(OUT, R38.1)
    */
}

NETLIST_START(k052535_dac)
{
    NET_MODEL("DI_BC847B NPN(IS=3.75f NF=1.00 BF=1.61k VAF=121 IKF=8.79m ISE=8.38p NE=2.00 BR=4.00 NR=1.00 VAR=24.0 IKR=90.0m RE=0.765 RB=3.06 RC=0.306 XTB=1.5 CJE=13.3p VJE=1.10 MJE=0.500 CJC=8.67p VJC=0.300 MJC=0.300 TF=520p TR=78.9n EG=1.12)")

    RES(R1, 220)
    RES(R2, 220)
    RES(R3, 220)
    RES(R4, 220)
    RES(R5, 220)

    RES(R6, RES_K(47))
    RES(R7, RES_K(20))
    RES(R8, RES_K(10))
    RES(R9, RES_K(4.7))
    RES(R10, RES_K(2.1))
    RES(R11, RES_K(13))
    RES(R12, 220)
    RES(R13, 470)

    QBJT_EB(Q1, "DI_BC847B")

    NET_C(VCC, R1.2, R2.2, R3.2, R4.2, R5.2, R11.2, Q1.C)
    NET_C(GND, R13.1)

    NET_C(D0, R1.1, R6.2)
    NET_C(D1, R2.1, R7.2)
    NET_C(D2, R3.1, R8.2)
    NET_C(D3, R4.1, R9.2)
    NET_C(D4, R5.1, R10.2)

    NET_C(BLANK, R6.1, R7.1, R8.1, R9.1, R10.1, R11.1, Q1.B)

    NET_C(Q1.E, R13.2, R12.2)
    ALIAS(OUT, R12.1)
}