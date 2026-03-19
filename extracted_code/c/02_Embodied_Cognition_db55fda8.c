/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 420
 * Language: c
 * Block ID: db55fda8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：混乱的布局增加认知负荷
void messy_function() {
int x=0;int y=0;for(int i=0;i<10;i++){if(x>5){y++;}else{x++;}}
}

// 正确：清晰的视觉结构
void clean_function(void) {
    int x = 0;
    int y = 0;

    for (int i = 0; i < 10; i++) {
        if (x > 5) {
            y++;
        } else {
            x++;
        }
    }
}
