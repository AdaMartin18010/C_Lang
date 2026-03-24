/*
 * C23 <math.h> 新增函数示例
 * 编译: gcc -std=c23 -o 11_new_math_functions 11_new_math_functions.c -lm
 *       clang -std=c23 -o 11_new_math_functions 11_new_math_functions.c -lm
 */

#include <stdio.h>
#include <math.h>

int main(void) {
    printf("C23 Math Library Extensions Demo\n\n");
    
    // 数学常量（不再需要自己定义）
    printf("Mathematical Constants:\n");
    printf("M_PI = %f\n", M_PI);
    printf("M_E = %f\n", M_E);
    printf("M_LOG2E = %f\n", M_LOG2E);
    printf("M_LOG10E = %f\n", M_LOG10E);
    printf("M_LN2 = %f\n", M_LN2);
    printf("M_LN10 = %f\n", M_LN10);
    printf("M_PI_2 = %f\n", M_PI_2);
    printf("M_PI_4 = %f\n", M_PI_4);
    printf("M_1_PI = %f\n", M_1_PI);
    printf("M_2_PI = %f\n", M_2_PI);
    printf("M_2_SQRTPI = %f\n", M_2_SQRTPI);
    printf("M_SQRT2 = %f\n", M_SQRT2);
    printf("M_SQRT1_2 = %f\n", M_SQRT1_2);
    
    // roundeven - 银行家舍入（四舍六入五成双）
    printf("\nroundeven (Banker's rounding):\n");
    printf("roundeven(2.5) = %.1f\n", roundeven(2.5));   // 2.0
    printf("roundeven(3.5) = %.1f\n", roundeven(3.5));   // 4.0
    printf("roundeven(2.4) = %.1f\n", roundeven(2.4));   // 2.0
    printf("roundeven(2.6) = %.1f\n", roundeven(2.6));   // 3.0
    
    // fminimum 和 fmaximum
    printf("\nfminimum and fmaximum:\n");
    printf("fminimum(3.0, 5.0) = %f\n", fminimum(3.0, 5.0));
    printf("fmaximum(3.0, 5.0) = %f\n", fmaximum(3.0, 5.0));
    
    // 处理 NaN 的行为
    printf("\nWith NaN handling:\n");
    printf("fminimum(3.0, NAN) = %f\n", fminimum(3.0, NAN));
    printf("fmaximum(NAN, 5.0) = %f\n", fmaximum(NAN, 5.0));
    
    // fminimum_num 和 fmaximum_num（忽略 NaN）
    printf("\nfminimum_num and fmaximum_num (ignore NaN):\n");
    printf("fminimum_num(3.0, NAN) = %f\n", fminimum_num(3.0, NAN));
    printf("fmaximum_num(NAN, 5.0) = %f\n", fmaximum_num(NAN, 5.0));
    
    // fminimum_mag 和 fmaximum_mag（按绝对值）
    printf("\nfminimum_mag and fmaximum_mag (by magnitude):\n");
    printf("fminimum_mag(-5.0, 3.0) = %f\n", fminimum_mag(-5.0, 3.0));  // 3.0 (|3| < |5|)
    printf("fmaximum_mag(-5.0, 3.0) = %f\n", fmaximum_mag(-5.0, 3.0));  // -5.0 (|5| > |3|)
    
    // powr - 实数幂（仅在正数定义域）
    printf("\npowr (real power, positive domain only):\n");
    printf("powr(2.0, 3.0) = %f\n", powr(2.0, 3.0));
    printf("powr(M_E, 1.0) = %f (should be e)\n", powr(M_E, 1.0));
    
    // pown - 整数幂
    printf("\npown (integer power):\n");
    printf("pown(2.0, 10) = %f\n", pown(2.0, 10));
    printf("pown(10.0, 3) = %f\n", pown(10.0, 3));
    
    // rootn - n次方根
    printf("\nrootn (n-th root):\n");
    printf("rootn(27.0, 3) = %f (cubic root)\n", rootn(27.0, 3));
    printf("rootn(16.0, 4) = %f (4th root)\n", rootn(16.0, 4));
    
    // compound - 复利计算
    printf("\ncompound (compound interest):\n");
    // compound(x, y) = (1+x)^y
    printf("Annual rate 5%% for 10 years: %.4f\n", compound(0.05, 10.0));
    printf("Annual rate 3%% for 20 years: %.4f\n", compound(0.03, 20.0));
    
    // annuity - 年金计算
    printf("\nannuity:\n");
    // annuity(x, y) = (1 - (1+x)^-y) / x
    printf("Annuity factor (5%%, 10 years): %.4f\n", annuity(0.05, 10.0));
    printf("Monthly payment factor (0.5%%/mo, 360 mo): %.4f\n", annuity(0.005, 360.0));
    
    // 实际应用：贷款计算
    printf("\n--- Loan Calculator Example ---\n");
    double principal = 300000.0;  // $300k loan
    double annual_rate = 0.045;    // 4.5% annual rate
    int years = 30;
    
    double monthly_rate = annual_rate / 12;
    int num_payments = years * 12;
    
    double monthly_payment = principal / annuity(monthly_rate, num_payments);
    double total_payment = monthly_payment * num_payments;
    double total_interest = total_payment - principal;
    
    printf("Loan amount: $%.2f\n", principal);
    printf("Annual rate: %.2f%%\n", annual_rate * 100);
    printf("Term: %d years\n", years);
    printf("Monthly payment: $%.2f\n", monthly_payment);
    printf("Total payment: $%.2f\n", total_payment);
    printf("Total interest: $%.2f\n", total_interest);
    
    printf("\nMath functions demo complete!\n");
    
    return 0;
}
