#' Multi-threaded, coercion-free implementations of common row-wise operations
#'
#' Fast row-oriented operations implemented in C++, all of which are multi-threaded by leveraging
#' \pkg{RcppParallel} and \pkg{RcppThread}. Thanks to Boost's `string_ref`, virtually no deep copies
#' of input data are made. In contrast to other functions, the ones in this package support data
#' frames with differently typed columns as input without coercion to a matrix, performing
#' on-the-fly type promotion following R rules, where necessary.
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
