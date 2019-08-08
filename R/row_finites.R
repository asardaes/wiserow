#' Check if a row's columns have finite values
#'
#' For each desired row, check if all/any/none of the columns have finite values, i.e. neither `Inf`
#' nor `NA`.
#'
#' @export
#' @templateVar par which_cols
#' @templateVar choices ("all", "any", "none")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_mode -na_action
#'
row_finites <- function(.data, which_cols = "none", ...) {
    UseMethod("row_finites")
}

#' @rdname row_finites
#' @export
#'
row_finites.matrix <- function(.data, which_cols = "none", ...) {
    which_cols <- match.arg(which_cols, c("all", "any", "none"))

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        bulk_bool_op = which_cols
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_finites, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_finites
#' @export
#'
row_finites.data.frame <- function(.data, which_cols = "none", ...) {
    which_cols <- match.arg(which_cols, c("all", "any", "none"))

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_mode = "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        bulk_bool_op = which_cols
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_finites, metadata, .data, ans, extras)
    }

    ans
}
