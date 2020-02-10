#' Check if a row's columns' values are present in a set of known values
#'
#' @export
#' @templateVar par match_type
#' @templateVar choices ("all", "any", "none", "which_first", "count")
#'
#' @template data-param
#' @template generic-choices
#' @param sets The list of sets to compare against. See details.
#' @param negate Logical. If `TRUE`, values that are *not* in the `sets` are sought.
#' @inheritDotParams op_ctrl -output_mode -na_action
#'
#' @details
#'
#' The `sets` are recycled in order to match the number of columns in `.data`.
#'
#' Type promotion/conversion will follow normal R rules.
#'
#' @note
#'
#' Note that string comparison follows C++ rules.
#'
row_in <- function(.data, match_type = "none", sets = list(), negate = FALSE, ...) {
    UseMethod("row_in")
}

#' @rdname row_in
#' @export
#'
row_in.matrix <- function(.data, match_type = "none", sets = list(), negate = FALSE, ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))

    if (length(sets) == 0L) {
        stop("The list of sets cannot be empty.")
    }
    else if (any(!sapply(sets, typeof) %in% .supported_modes)) {
        stop(paste0("The types of the values in the sets must be some of: ",
                    paste(.supported_modes, collapse = ", ")))
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = if (match_type %in% c("which_first", "count")) "integer" else "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type,
        target_sets = sets,
        negate = isTRUE(negate)
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_in, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_in
#' @export
#'
row_in.data.frame <- function(.data, match_type = "none", sets = list(), negate = FALSE, ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))

    if (length(sets) == 0L) {
        stop("The list of sets cannot be empty.")
    }
    else if (any(!sapply(sets, typeof) %in% .supported_modes)) {
        stop(paste0("The types of the values in the sets can only be one of: ",
                    paste(.supported_modes, collapse = ", ")))
    }

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_mode = if (match_type %in% c("which_first", "count")) "integer" else "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type,
        target_sets = sets,
        negate = isTRUE(negate)
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_in, metadata, .data, ans, extras)
    }

    ans
}
