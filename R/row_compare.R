#' Check if a row's columns fulfill a given comparison
#'
#' For each desired row, check if all/any/none of the columns fulfill a given comparison, or return
#' the index of the first column that fulfills the comparison.
#'
#' @export
#' @templateVar par match_type
#' @templateVar choices ("all", "any", "none", "which_first")
#'
#' @template data-param
#' @template generic-choices
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
#' Note that `match_type = "which_first"` will result in an integer output, whereas the other
#' options result in a logical output.
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
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first"))
    operator <- match.arg(operator, .supported_comp_operators)
    if (operator == "is") operator <- "=="

    values <- as.list(values)
    if (any(lengths(values) != 1L)) {
        stop("Each value in 'values' must have length equal to 1.")
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = if (match_type == "which_first") "integer" else "logical",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type,
        comp_op = operator,
        target_val = values
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_compare, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_compare
#' @export
#'
row_compare.data.frame <- function(.data, match_type = "none", operator = "==", values = 0L, ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first"))
    operator <- match.arg(operator, .supported_comp_operators)
    if (operator == "is") operator <- "=="

    values <- as.list(values)
    if (any(lengths(values) != 1L)) {
        stop("Each value in 'values' must have length equal to 1.")
    }

    dots <- list(...)
    if (is.null(dots$input_modes)) {
        dots$input_modes <- sapply(.data, typeof)
    }

    dots <- c(dots, list(
        input_class = "data.frame",
        output_mode = if (match_type == "which_first") "integer" else "logical"
    ))

    metadata <- do.call(op_ctrl, dots)
    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type,
        comp_op = operator,
        target_val = values
    )

    if (nrow(.data) > 0L) {
        .Call(C_row_compare, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_compare
#' @export
#' @importFrom data.table .SD
#'
row_compare.data.table <- function(.data, ...) {
    input_modes <- unlist(.data[, lapply(.SD, typeof)])
    NextMethod("row_compare", .data, input_modes = input_modes)
}
