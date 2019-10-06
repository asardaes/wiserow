#' Conditions related to infinite values
#'
#' @export
#' @templateVar par match_type
#' @templateVar choices ("all", "any", "none", "which_first", "count")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_mode -na_action -factor_mode
#'
row_infs <- function(.data, match_type = "none", ...) {
    UseMethod("row_infs")
}

#' @rdname row_infs
#' @export
#'
row_infs.matrix <- function(.data, match_type = "none", ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))
    output_mode <- if (match_type %in% c("which_first", "count")) "integer" else "logical"

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
        .Call(C_row_infs, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_infs
#' @export
#'
row_infs.data.frame <- function(.data, match_type = "none", ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))
    output_mode <- if (match_type %in% c("which_first", "count")) "integer" else "logical"

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_mode = output_mode,
                        na_action = "pass",
                        factor_mode = "integer",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_infs, metadata, .data, ans, extras)
    }

    ans
}
