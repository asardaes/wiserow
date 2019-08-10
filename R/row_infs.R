#' Check if a row's columns have infinite values
#'
#' For each desired row, check if all/any/none of the columns have infinite values (`Inf`).
#'
#' @export
#' @templateVar par which_cols
#' @templateVar choices ("all", "any", "none")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_mode -na_action
#'
row_infs <- function(.data, which_cols = "none", ...) {
    UseMethod("row_infs")
}

#' @rdname row_infs
#' @export
#'
row_infs.matrix <- function(.data, which_cols = "none", ...) {
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
        .Call(C_row_infs, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_infs
#' @export
#'
row_infs.data.frame <- function(.data, which_cols = "none", ...) {
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
        .Call(C_row_infs, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_infs
#' @export
#' @importFrom data.table .SD
#'
row_infs.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_infs", .data, input_modes = input_modes)
}
