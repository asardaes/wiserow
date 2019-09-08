#' Row-wise arithmetic operations
#'
#' @export
#'
#' @template data-param
#' @inheritDotParams op_ctrl -output_mode -factor_mode
#' @param operator One of ("+", "-", "*", "/").
#' @param output_mode Output's [base::storage.mode()]. If missing, it will be inferred.
#'
#' @details
#'
#' Conceptually, this function takes each row, say `x`, and executes `Reduce(operator, unlist(x))`.
#' `x` may be a list of values with different non-character types (like a `data.frame` row), and
#' `NA` values can be excluded.
#'
row_arith <- function(.data, ...) {
    UseMethod("row_arith")
}

#' @rdname row_arith
#' @export
#' @importFrom rlang is_missing
#'
row_arith.matrix <- function(.data, operator = c("+", "-", "*", "/"), output_mode, ...) {
    operator <- match.arg(operator)

    out_mode_missing <- rlang::is_missing(output_mode)
    if (out_mode_missing) {
        output_mode <- typeof(.data)
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        ...)

    if (out_mode_missing && metadata$output_mode %in% c("integer", "logical")) {
        metadata$output_mode <- if (operator == "/") "double" else "integer"
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    if (nrow(.data) > 0L) {
        .Call(C_row_arith, metadata, .data, ans, list(arith_op = operator))
    }

    ans
}

#' @rdname row_arith
#' @export
#' @importFrom rlang is_missing
#'
row_arith.data.frame <- function(.data, operator = c("+", "-", "*", "/"), output_mode, ...) {
    operator <- match.arg(operator)

    dots <- list(...)
    if (is.null(dots$input_modes)) {
        dots$input_modes <- sapply(.data, typeof)
    }

    out_mode_missing <- rlang::is_missing(output_mode)
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
                metadata$output_mode <- typeof(Reduce(operator, sapply(input_modes[cols][supported], vector, length = 1L)))
            }
        }
    }

    ans <- prepare_output(.data, metadata)

    if (nrow(.data) > 0L) {
        .Call(C_row_arith, metadata, .data, ans, list(arith_op = operator))
    }

    ans
}

#' @rdname row_arith
#' @export
#' @importFrom data.table .SD
#'
row_arith.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_arith", .data, input_modes = input_modes)
}
