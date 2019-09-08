#' Row-wise sum
#'
#' @export
#'
#' @template data-param
#' @inheritDotParams op_ctrl -output_mode -factor_mode
#' @param output_mode Output's [base::storage.mode()]. If missing, it will be inferred.
#'
row_sums <- function(.data, ...) {
    UseMethod("row_sums")
}

#' @rdname row_sums
#' @export
#' @importFrom rlang maybe_missing
#'
row_sums.matrix <- function(.data, output_mode, ...) {
    row_arith.matrix(.data, "+", rlang::maybe_missing(output_mode), ...)
}

#' @rdname row_sums
#' @export
#' @importFrom rlang maybe_missing
#'
row_sums.data.frame <- function(.data, output_mode, ...) {
    row_arith.data.frame(.data, "+", rlang::maybe_missing(output_mode), ...)
}

#' @rdname row_sums
#' @export
#' @importFrom data.table .SD
#'
row_sums.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_sums", .data, input_modes = input_modes)
}
