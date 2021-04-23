[![CRAN Status](http://www.r-pkg.org/badges/version/wiserow)](https://cran.r-project.org/package=wiserow)
[![R-CMD-check](https://github.com/asardaes/wiserow/workflows/R-CMD-check/badge.svg)](https://github.com/asardaes/wiserow/actions)
[![codecov](https://codecov.io/gh/asardaes/wiserow/branch/master/graph/badge.svg)](https://codecov.io/gh/asardaes/wiserow)

# Wiserow

Multi-threaded, coercion-free implementations of common row-wise operations.

Fast row-oriented operations implemented in C++, all of which are multi-threaded by
leveraging `RcppParallel` and `RcppThread`. Virtually no deep copies of input data are made,
even of character data thanks to Boost's `string_ref`. In contrast to other functions, the ones
in this package support data frames with differently typed columns as input without coercion to
a matrix, performing on-the-fly type promotion following R rules, where necessary.

## License

[GNU General Public License v3.0](LICENSE)

This software package was developed independently of any organization or
institution that is or has been associated with the author.
