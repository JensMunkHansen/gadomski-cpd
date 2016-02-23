// cpd - Coherent Point Drift
// Copyright (C) 2016 Pete Gadomski <pete.gadomski@gmail.com>
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

#include "cpd/registration.hpp"

namespace cpd {

double default_sigma2(const MatrixRef X, const MatrixRef Y) {
    assert(X.cols() == Y.cols());
    auto N = X.rows();
    auto M = Y.rows();
    auto D = X.cols();
    return (N * (X.transpose() * X).trace() + M * (Y.transpose() * Y).trace() -
            2 * X.colwise().sum() * Y.colwise().sum().transpose()) /
           (N * M * D);
}
}
