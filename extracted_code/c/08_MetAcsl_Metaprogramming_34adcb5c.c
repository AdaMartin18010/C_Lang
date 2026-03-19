/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 352
 * Language: c
 * Block ID: 34adcb5c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @meta infer_invariants {
    strategy: "template_based",
    templates: [
      // 数组遍历模式
      {
        match: "for(i=0; i<n; i++) process(arr[i])",
        invariant: [
          "0 <= i <= n",
          "\forall j; 0<=j<i ==> processed(arr[j])"
        ]
      },
      
      // 累加模式
      {
        match: "sum += expr",
        invariant: [
          "sum == \sum(0,i-1,\lambda k; original_expr[k])"
        ]
      },
      
      // 搜索模式
      {
        match: "if (found) break",
        invariant: [
          "!found ==> \forall j; 0<=j<i ==> not_target(arr[j])",
          "found ==> target(arr[i-1])"
        ]
      }
    ]
  }
*/

// 实际应用
int array_sum(const int* arr, int n) {
    int sum = 0;
    // MetAcsl自动生成:
    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant sum == \sum(0,i-1,\lambda integer k; arr[k]);
      @loop assigns i, sum;
      @loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}
