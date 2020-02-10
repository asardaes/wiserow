#' Row-wise sum
#'
#' Wrapper for [row_arith()] with `operator = "+"`.
#'
#' @export
#'
#' @template data-param
#' @param ... Arguments for [row_arith()] (except `operator`).
#'
row_sums <- function(.data, ...) {
    UseMethod("row_sums")
}

#' @rdname row_sums
#' @export
#'
row_sums.matrix <- function(.data, ...) {
    row_arith.matrix(.data, operator = "+", ...)
}

#' @rdname row_sums
#' @export
#'
row_sums.data.frame <- function(.data, ...) {
    row_arith.data.frame(.data, operator = "+", ...)
}
