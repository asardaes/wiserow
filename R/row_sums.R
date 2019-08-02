#' Row-wise sum
#'
#' @export
#'
#' @param .data A two-dimensional data structure.
#' @param ... Parameters for [op_ctrl()].
#' @param output_mode Output's [base::storage.mode()]. If missing, it will be inferred.
#'
row_sums <- function(.data, ...) {
    UseMethod("row_sums")
}

#' @rdname row_sums
#' @export
#'
row_sums.matrix <- function(.data, output_mode, ...) {
    validate_dim(.data)

    if (missing(output_mode)) {
        output_mode <- typeof(.data)
    }

    metadata <- op_ctrl(data_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        ...)

    if (metadata$output_mode == "logical") {
        metadata$output_mode <- "integer"
    }

    .Call(C_row_sums, metadata, .data)
}
