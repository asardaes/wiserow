#' Check if a row's columns have missing values
#'
#' For each desired row, check if all/any/none of the columns have missing values (`NA`), or return
#' the index of the first column containing `NA`.
#'
#' @export
#' @templateVar par match_type
#' @templateVar choices ("all", "any", "none", "which_first")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_mode -na_action
#'
#' @details
#'
#' Note that `match_type = "which_first"` will result in an integer output, whereas the other
#' options result in a logical output.
#'
row_nas <- function(.data, match_type = "none", ...) {
    UseMethod("row_nas")
}

#' @rdname row_nas
#' @export
#'
row_nas.matrix <- function(.data, match_type = "none", ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first"))
    output_mode <- if (match_type == "which_first") "integer" else "logical"

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        na_action = "pass",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_nas, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_nas
#' @export
#'
row_nas.data.frame <- function(.data, match_type = "none", ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first"))
    output_mode <- if (match_type == "which_first") "integer" else "logical"

    dots <- list(...)
    if (is.null(dots$input_modes)) {
        dots$input_modes <- sapply(.data, typeof)
    }

    dots <- c(dots, list(
        input_class = "data.frame",
        output_mode = output_mode,
        na_action = "pass"
    ))

    metadata <- do.call(op_ctrl, dots)
    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type
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
