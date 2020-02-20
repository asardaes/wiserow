#' Multi-threaded, coercion-free implementations of common row-wise operations
#'
#' Fast row-oriented operations implemented in C++, all of which are multi-threaded by leveraging
#' \pkg{RcppParallel} and \pkg{RcppThread}. Virtually no deep copies of input data are made, even of
#' character data thanks to the 'string_ref' class from the C++ 'Boost' library. In contrast to
#' other functions, the ones in this package support data frames with differently typed columns as
#' input without coercing to a matrix, performing, if necessary, on-the-fly type promotion according
#' to R rules (like transforming logicals to integers to allow summation).
#'
#' Start by looking at [op_ctrl()].
#'
#' @author Alexis Sarda-Espinosa
#'
#' @useDynLib wiserow, .registration = TRUE
#' @importFrom Rcpp LdFlags
#' @importFrom RcppParallel RcppParallelLibs
#'
"_PACKAGE"

.onUnload <- function(libpath) {
    library.dynam.unload("wiserow", libpath)
}
