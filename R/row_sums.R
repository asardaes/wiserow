#' Row-wise sum
#'
#' @export
#'
#' @param .data A two-dimensional data structure.
#' @inheritDotParams op_ctrl -output_mode
#' @param output_mode Output's [base::storage.mode()]. If missing, it will be inferred.
#'
row_sums <- function(.data, ...) {
    UseMethod("row_sums")
}

#' @rdname row_sums
#' @export
#'
row_sums.matrix <- function(.data, output_mode, ...) {
    if (missing(output_mode)) {
        output_mode <- typeof(.data)
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        ...)

    if (metadata$output_mode == "logical") {
        metadata$output_mode <- "integer"
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    if (nrow(.data) > 0L) {
        .Call(C_row_sums, metadata, .data, ans)
    }

    ans
}
