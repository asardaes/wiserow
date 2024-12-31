#' Check if a row's columns fulfill a given comparison
#'
#' For each desired row, check if all/any/none of the columns fulfill a given comparison, return the
#' index of the first column that fulfills the comparison, or the number of columns that do.
#'
#' @export
#'
#' @param .data `r roxygen_data_param()`
#' @param match_type `r roxygen_generic_choices('("all", "any", "none", "which_first", "count")')`
#' @param operator A character indicating the comparison operator. See details.
#' @param values The value or list of values to compare against. See details.
#' @inheritDotParams op_ctrl -output_mode
#'
#' @details
#'
#' The character in `operator` should match one of the [base::Comparison] operators. You can also
#' pass "is", which is an alias for "==", but might convey code semantics better in case `values`
#' contain `NA` or `Inf`.
#'
#' If `values` is not a list, it will be coerced to one with [base::as.list()]. Each element in the
#' list must have length equal to 1. The values are recycled in order to match the number of columns
#' in `.data`.
#'
#' Type promotion/conversion will follow normal R rules.
#'
#' The `na_action` from [op_ctrl()] is only relevant when a value from `values` is *not* `NA`.
#'
#' @note
#'
#' Note that string comparison follows C++ rules.
#'
#' @examples
#'
#' row_compare(data.frame(0L, 1.0, "0", TRUE, 0+0i),
#'             "all", "==", values = list(0L, 1L))
#'
#' row_compare(data.frame(NA_integer_, NA_real_, NA_character_, NA, NA_complex_),
#'             "all", "is", values = NA)
#'
row_compare <- function(.data, match_type = "none", operator = "==", values = 0L, ...) {
    UseMethod("row_compare")
}

#' @rdname row_compare
#' @export
#'
row_compare.matrix <- function(.data, match_type = "none", operator = "==", values = 0L, ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))
    operator <- match.arg(operator, .supported_comp_operators)
    if (operator == "is") operator <- "=="

    values <- as.list(values)
    if (any(lengths(values) != 1L)) {
        stop("Each value in 'values' must have length equal to 1.")
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = if (match_type %in% c("which_first", "count")) "integer" else "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type,
        comp_op = operator,
        target_val = values
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_compare, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_compare
#' @export
#'
row_compare.data.frame <- function(.data, match_type = "none", operator = "==", values = 0L, ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))
    operator <- match.arg(operator, .supported_comp_operators)
    if (operator == "is") operator <- "=="

    values <- as.list(values)
    if (any(lengths(values) != 1L)) {
        stop("Each value in 'values' must have length equal to 1.")
    }

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_mode = if (match_type %in% c("which_first", "count")) "integer" else "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type,
        comp_op = operator,
        target_val = values
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_compare, metadata, .data, ans, extras)
    }

    ans
}
