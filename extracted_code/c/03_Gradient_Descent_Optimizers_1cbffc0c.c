/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 22
 * Language: c
 * Block ID: 1cbffc0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// BGD伪代码
for epoch in range(num_epochs):
    gradient = compute_gradient_full_dataset(X, y, theta)
    theta = theta - learning_rate * gradient
