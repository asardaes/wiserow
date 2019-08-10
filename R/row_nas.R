#' Check if a row's columns have missing values
#'
#' For each desired row, check if all/any/none of the columns have missing values (`NA`).
#'
#' @export
#' @templateVar par which_cols
#' @templateVar choices ("all", "any", "none")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_mode -na_action
#'
row_nas <- function(.data, which_cols = "none", ...) {
    UseMethod("row_nas")
}

#' @rdname row_nas
#' @export
#'
row_nas.matrix <- function(.data, which_cols = "none", ...) {
    which_cols <- match.arg(which_cols, c("all", "any", "none"))

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = "logical",
                        na_action = "pass",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        bulk_bool_op = which_cols
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_nas, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_nas
#' @export
#'
row_nas.data.frame <- function(.data, which_cols = "none", ...) {
    which_cols <- match.arg(which_cols, c("all", "any", "none"))

    dots <- list(...)
    if (is.null(dots$input_modes)) {
        dots$input_modes <- sapply(.data, typeof)
    }

    dots <- c(dots, list(
        input_class = "data.frame",
        output_mode = "logical",
        na_action = "pass"
    ))

    metadata <- do.call(op_ctrl, dots)
    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        bulk_bool_op = which_cols
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_nas, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_nas
#' @export
#' @importFrom data.table .SD
#'
row_nas.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_nas", .data, input_modes = input_modes)
}
