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
#'
row_sums.matrix <- function(.data, output_mode, ...) {
    out_mode_missing <- missing(output_mode)
    if (out_mode_missing) {
        output_mode <- typeof(.data)
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        ...)

    if (out_mode_missing && metadata$output_mode == "logical") {
        metadata$output_mode <- "integer"
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    if (nrow(.data) > 0L) {
        .Call(C_row_sums, metadata, .data, ans)
    }

    ans
}

#' @rdname row_sums
#' @export
#'
row_sums.data.frame <- function(.data, output_mode, ...) {
    dots <- list(...)
    if (is.null(dots$input_modes)) {
        dots$input_modes <- sapply(.data, typeof)
    }

    out_mode_missing <- missing(output_mode)

    if (out_mode_missing) {
        output_mode <- "integer"
    }

    dots <- c(dots, list(
        input_class = "data.frame",
        output_mode = output_mode,
        factor_mode = "integer"
    ))

    metadata <- do.call(op_ctrl, dots)
    metadata <- validate_metadata(.data, metadata)

    if (out_mode_missing) {
        input_modes <- metadata$input_modes
        if (length(input_modes) > 0L) {
            cols <- if (is.null(metadata$cols)) seq_along(input_modes) else metadata$cols
            supported <- input_modes[cols] != "character"

            if (any(supported)) {
                metadata$output_mode <- typeof(sum(sapply(input_modes[cols][supported], vector, length = 1L)))
            }
        }
    }

    ans <- prepare_output(.data, metadata)

    if (nrow(.data) > 0L) {
        .Call(C_row_sums, metadata, .data, ans)
    }

    ans
}

#' @rdname row_sums
#' @export
#' @importFrom data.table .SD
#'
row_sums.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_sums", .data, input_modes = input_modes)
}
