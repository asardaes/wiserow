#' Row-wise sum
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

#' @rdname row_sums
#' @export
#' @importFrom data.table .SD
#'
row_sums.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_sums", .data, input_modes = input_modes)
}
