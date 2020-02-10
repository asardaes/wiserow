#' Row-wise means
#'
#' @export
#'
#' @template data-param
#' @inheritDotParams op_ctrl -output_mode -output_class -factor_mode
#' @param cumulative Logical. Whether to return the cumulative operation.
#' @param output_mode Passed to [op_ctrl()]. If missing, it will be inferred.
#' @param output_class Passed to [op_ctrl()]. If missing, it will be inferred.
#'
#' @examples
#'
#' mat <- matrix(rnorm(10L), nrow = 2L, ncol = 5L)
#'
#' row_means(mat)
#'
#' # semantically, this is like looking for row means that are different from zero
#' # (but not missing)
#' mat[1L] <- NA_real_
#' row_means(mat, output_mode = "logical", na_action = "pass")
#'
row_means <- function(.data, ...) {
    UseMethod("row_means")
}

#' @rdname row_means
#' @export
#'
row_means.matrix <- function(.data, cumulative = FALSE, output_mode, output_class, ...) {
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
        metadata$output_mode <- "double"
    }

    if (cumulative && !metadata$output_class %in% c("matrix", "data.frame")) {
        stop("A cumulative operation requires a matrix or data.frame output class.")
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata, TRUE)
    extras <- list(
        cumulative = cumulative
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_means, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_means
#' @export
#'
row_means.data.frame <- function(.data, cumulative = FALSE, output_mode, output_class, ...) {
    out_mode_missing <- missing(output_mode)
    if (out_mode_missing) {
        output_mode <- "double"
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
                metadata$output_mode <- typeof(Reduce("+", sapply(input_modes[cols][supported], vector, length = 1L)))
                if (out_mode_missing && metadata$output_mode %in% c("integer", "logical")) {
                    metadata$output_mode <- "double"
                }
            }
        }
    }

    if (cumulative && !metadata$output_class %in% c("matrix", "data.frame")) {
        stop("A cumulative operation requires a matrix or data.frame output class.")
    }

    ans <- prepare_output(.data, metadata, TRUE)
    extras <- list(
        cumulative = cumulative
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_means, metadata, .data, ans, extras)
    }

    ans
}
