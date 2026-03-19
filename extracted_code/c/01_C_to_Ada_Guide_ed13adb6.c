/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 106
 * Language: c
 * Block ID: ed13adb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// if-else
if (x > 0) {
    printf("positive");
} else if (x < 0) {
    printf("negative");
} else {
    printf("zero");
}

// for循环
for (int i = 0; i < 10; i++) {
    process(i);
}

// while循环
while (count < 100) {
    count++;
}

// switch
case (cmd) {
    case 'A': action_a(); break;
    case 'B': action_b(); break;
    default: error();
}
