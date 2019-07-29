#' Row-wise sum
#'
#' @export
#'
#' @param .data A two-dimensional data structure.
#' @param ... Currently ignored.
#' @param output_mode Output's [base::storage.mode()].
#'
row_sums <- function(.data, ...) {
    UseMethod("row_sums")
}

#' @rdname row_sums
#' @export
#'
row_sums.matrix <- function(.data, output_mode = storage.mode(.data), ...) {
    validate_dim(.data)

    metadata <- list(input_class = "matrix",
                     input_modes = typeof(.data),
                     output_mode = output_mode,
                     num_workers = num_workers())

    .Call(C_row_sums, metadata, .data)
}
