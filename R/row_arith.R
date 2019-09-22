#' Row-wise arithmetic operations
#'
#' @export
#'
#' @template data-param
#' @inheritDotParams op_ctrl -output_mode -output_class -factor_mode
#' @param operator One of ("+", "-", "*", "/").
#' @param cumulative Logical. Whether to return the cumulative operation.
#' @param output_mode Passed to [op_ctrl()]. If missing, it will be inferred.
#' @param output_class Passed to [op_ctrl()]. If missing, it will be inferred.
#'
#' @details
#'
#' Conceptually, this function takes each row, say `x`, and executes `Reduce(operator, unlist(x))`.
#' `x` may be a list of values with different non-character types (like a `data.frame` row), `NA`
#' values can be excluded, and results can be accumulated.
#'
row_arith <- function(.data, ...) {
    UseMethod("row_arith")
}

#' @rdname row_arith
#' @export
#' @importFrom rlang is_missing
#'
row_arith.matrix <- function(.data, operator = c("+", "-", "*", "/"), cumulative = FALSE, output_mode, output_class, ...) {
    operator <- match.arg(operator)

    out_mode_missing <- rlang::is_missing(output_mode)
    if (out_mode_missing) {
        output_mode <- typeof(.data)
    }

    if (rlang::is_missing(output_class)) {
        output_class <- if (cumulative) "matrix" else "vector"
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        output_class = output_class,
                        ...)

    if (out_mode_missing && metadata$output_mode %in% c("integer", "logical")) {
        metadata$output_mode <- if (operator == "/") "double" else "integer"
    }

    if (cumulative && !metadata$output_class %in% c("matrix", "data.frame")) {
        stop("A cumulative operation requires a matrix or data.frame output class.")
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata, TRUE)
    extras <- list(
        arith_op = operator,
        cumulative = cumulative
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_arith, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_arith
#' @export
#' @importFrom rlang is_missing
#'
row_arith.data.frame <- function(.data, operator = c("+", "-", "*", "/"), cumulative = FALSE, output_mode, output_class, ...) {
    operator <- match.arg(operator)

    dots <- list(...)
    if (is.null(dots$input_modes)) {
        dots$input_modes <- sapply(.data, typeof)
    }

    out_mode_missing <- rlang::is_missing(output_mode)
    if (out_mode_missing) {
        output_mode <- "integer"
    }

    if (rlang::is_missing(output_class)) {
        output_class <- if (cumulative) "data.frame" else "vector"
    }

    dots <- c(dots, list(
        input_class = "data.frame",
        output_mode = output_mode,
        output_class = output_class,
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

    if (cumulative && !metadata$output_class %in% c("matrix", "data.frame")) {
        stop("A cumulative operation requires a matrix or data.frame output class.")
    }

    ans <- prepare_output(.data, metadata, TRUE)
    extras <- list(
        arith_op = operator,
        cumulative = cumulative
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_arith, metadata, .data, ans, extras)
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
