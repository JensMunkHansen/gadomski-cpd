// cpd - Coherent Point Drift
// Copyright (C) 2017 Pete Gadomski <pete.gadomski@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <cpd/affine.hpp>

namespace cpd {

template <typename M, typename V>
AffineResult<M, V> Affine<M, V>::compute_one(
    const M& fixed, const M& moving, const Probabilities<M, V>& probabilities,
    typename M::Scalar sigma2) const {
    typename M::Scalar np = probabilities.p1.sum();
    V mu_x = fixed.transpose() * probabilities.pt1 / np;
    V mu_y = moving.transpose() * probabilities.p1 / np;
    M b1 = probabilities.px.transpose() * moving - np * mu_x * mu_y.transpose();
    M b2 =
        (moving.array() * probabilities.p1.replicate(1, moving.cols()).array())
                .transpose()
                .matrix() *
            moving -
        np * mu_y * mu_y.transpose();
    AffineResult<M, V> result;
    result.transform = b1 * b2.inverse();
    result.translation = mu_x - result.transform * mu_y;
    result.sigma2 =
        std::abs((fixed.array().pow(2) *
                  probabilities.pt1.replicate(1, fixed.cols()).array())
                     .sum() -
                 np * mu_x.transpose() * mu_x -
                 (b1 * result.transform.transpose()).trace()) /
        (np * fixed.cols());
    result.points = moving * result.transform.transpose() +
                    result.translation.transpose().replicate(moving.rows(), 1);
    return result;
}

template <typename M, typename V>
M AffineResult<M, V>::matrix() const {
    typename M::Index rows = transform.rows() + 1;
    typename M::Index cols = transform.cols() + 1;
    M matrix = M::Zero(rows, cols);
    for (typename M::Index row = 0; row < transform.rows(); ++row) {
        for (typename M::Index col = 0; col < transform.cols(); ++col) {
            matrix(row, col) = transform(row, col);
        }
        matrix(row, cols - 1) = translation(row);
    }
    matrix(rows - 1, cols - 1) = 1;
    return matrix;
}

template <typename M, typename V>
void AffineResult<M, V>::denormalize(const Normalization<M, V>& normalization) {
    Result<M, V>::denormalize(normalization);
    translation = normalization.fixed_scale * translation +
                  normalization.fixed_mean -
                  transform * normalization.moving_mean;
    transform =
        transform * normalization.fixed_scale / normalization.moving_scale;
}

template <typename M, typename V>
AffineResult<M, V> affine(const M& fixed, const M& moving) {
    Affine<M, V> affine;
    return affine.run(fixed, moving);
}

template class Affine<Matrix, Vector>;
template class AffineResult<Matrix, Vector>;
template AffineResult<Matrix, Vector> affine(const Matrix& fixed,
                                             const Matrix& moving);

template class Affine<MatrixF, VectorF>;
template class AffineResult<MatrixF, VectorF>;
template AffineResult<MatrixF, VectorF> affine(const MatrixF& fixed,
                                               const MatrixF& moving);

template class GaussTransform<Matrix, Vector>;
std::unique_ptr<GaussTransform<Matrix, Vector>> dummyAffine;

} // namespace cpd
