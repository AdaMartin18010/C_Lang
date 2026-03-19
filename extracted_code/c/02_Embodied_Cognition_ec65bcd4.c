/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 443
 * Language: c
 * Block ID: ec65bcd4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：过度抽象导致心智模型断裂
void process(Thing *t) {
    do_something(t);  // 不知道在做什么
    transform(t);     // 不知道转换了什么
    finalize(t);      // 不知道完成了什么
}

// 正确：具体命名支持心智模型
void process_user_registration(User *user) {
    validate_email(user->email);
    hash_password(user->password);
    send_welcome_email(user);
}
