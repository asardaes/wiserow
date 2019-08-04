#' Check if columns have missing values
#'
#' @export
#'
#' @param .data A two-dimensional data structure.
#' @inheritDotParams op_ctrl -output_mode -na_action
#' @param condition One of ("all", "any", "none"). Possibly abbreviated.
#'
cols_have_na <- function(.data, ...) {
    UseMethod("cols_have_na")
}

#' @rdname cols_have_na
#' @export
#'
cols_have_na.matrix <- function(.data, condition = "none", ...) {
    condition <- match.arg(condition, c("all", "any", "none"))

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = "logical",
                        na_action = "pass",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        bulk_comp_op = condition
    )

    if (nrow(.data) > 0L) {
        .Call(C_cols_have_na, metadata, .data, ans, extras)
    }

    ans
}
