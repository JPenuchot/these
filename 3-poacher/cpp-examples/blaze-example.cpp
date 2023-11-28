#include <blaze/Blaze.h>
#include <blaze/math/dense/DynamicMatrix.h>
#include <blaze/math/dense/DynamicVector.h>

#include <iostream>

blaze::DynamicMatrix<float> a(8192, 8192, 1.f);
blaze::DynamicVector<float> b(8192, 2.f);
blaze::DynamicVector<float> c(8192, 3.f);

blaze::DVecDVecAddExpr<
    blaze::DMatDVecMultExpr<
        blaze::DynamicMatrix<float>,
        blaze::DynamicVector<float>>,
    blaze::DynamicVector<float>, false>
    expr = a * b + c;

int main() {
  blaze::DynamicVector<float> res = expr;
  std::cout << res << '\n';
}
