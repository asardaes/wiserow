#' Check if rows have missing values
#'
#' For each desired row, check if all/any/none of the columns have missing values (`NA`).
#'
#' @export
#'
#' @param .data A two-dimensional data structure.
#' @inheritDotParams op_ctrl -output_mode -na_action
#' @param condition One of ("all", "any", "none"). Possibly abbreviated.
#'
row_nas <- function(.data, ...) {
    UseMethod("row_nas")
}

#' @rdname row_nas
#' @export
#'
row_nas.matrix <- function(.data, condition = "none", ...) {
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
        .Call(C_row_nas, metadata, .data, ans, extras)
    }

    ans
}
