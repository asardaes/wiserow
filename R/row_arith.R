#' Row-wise arithmetic operations
#'
#' @export
#'
#' @param .data `r roxygen_data_param()`
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
#' @examples
#'
#' mat <- matrix(1L:9L, nrow = 3L, ncol = 3L)
#'
#' row_arith(mat, "-")
#' row_arith(mat, "-", cumulative = TRUE)
#'
#' df <- data.frame(bool = TRUE, int = 1L, double = 1.0, complex = 1+0i)
#'
#' # all columns promoted to complex
#' row_arith(df, "*")
#' row_arith(df, "/", cumulative = TRUE)
#'
#' # only promotion to integer is needed
#' row_arith(df, "+", cols = 1:2)
#'
row_arith <- function(.data, ...) {
    UseMethod("row_arith")
}

#' @rdname row_arith
#' @export
#'
row_arith.matrix <- function(.data, operator = c("+", "-", "*", "/"), cumulative = FALSE, output_mode, output_class, ...) {
    operator <- match.arg(operator)

    out_mode_missing <- missing(output_mode)
    if (out_mode_missing) {
        output_mode <- typeof(.data)
    }

    if (missing(output_class)) {
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

    if (NROW(ans) > 0L) {
        .Call(C_row_arith, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_arith
#' @export
#'
row_arith.data.frame <- function(.data, operator = c("+", "-", "*", "/"), cumulative = FALSE, output_mode, output_class, ...) {
    operator <- match.arg(operator)

    out_mode_missing <- missing(output_mode)
    if (out_mode_missing) {
        output_mode <- "integer"
    }

    if (missing(output_class)) {
        output_class <- if (cumulative) "data.frame" else "vector"
    }

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_class = output_class,
                        output_mode = output_mode,
                        factor_mode = "integer",
                        ...)

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

    if (NROW(ans) > 0L) {
        .Call(C_row_arith, metadata, .data, ans, extras)
    }

    ans
}
