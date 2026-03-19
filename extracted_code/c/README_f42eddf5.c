/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\README.md
 * Line: 760
 * Language: c
 * Block ID: f42eddf5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Bell 态 |Φ+⟩ = (|00⟩ + |11⟩) / √2
 */

void bell_state_demo(void)
{
    QSimulator qs;
    qs_init(&qs, 2);

    printf("Initial state: |00⟩\n");

    // 对第一个量子比特应用 H
    qs_hadamard(&qs, 0);
    printf("After H on q0:\n");
    qs_print_state(&qs);

    // CNOT(0, 1)
    qs_cnot(&qs, 0, 1);
    printf("After CNOT(0, 1):\n");
    qs_print_state(&qs);

    // 多次测量验证纠缠
    printf("\nMeasurement statistics:\n");
    int counts[4] = {0};

    for (int run = 0; run < 1000; run++) {
        QSimulator qscopy = qs;
        int m0 = qs_measure(&qscopy, 0);
        int m1 = qs_measure(&qscopy, 1);
        counts[m0 * 2 + m1]++;
    }

    printf("|00⟩: %d (expected ~50%%)\n", counts[0]);
    printf("|01⟩: %d (expected ~0%%)\n", counts[1]);
    printf("|10⟩: %d (expected ~0%%)\n", counts[2]);
    printf("|11⟩: %d (expected ~50%%)\n", counts[3]);
}
