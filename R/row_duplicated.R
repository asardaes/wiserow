#' Conditions related to duplicated values
#'
#' @export
#' @templateVar par match_type
#' @templateVar choices (`NULL`, "any", "none", "which_first", "count")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_class -output_mode -na_action
#' @param output_class Passed to [op_ctrl()]. If missing, it will be inferred.
#'
#' @details
#'
#' For each row, different checks can be performed on whether there are duplicated values among the
#' columns. The default (when `match_type = NULL`) returns a logical matrix/data.frame with the same
#' number of columns as the input, where each column indicates whether the corresponding input
#' column is duplicated for that row. Other `match_type` values perform aggregations and are
#' self-explanatory.
#'
#' Care must be taken for input data frames whose columns have different types. Type promotion will
#' follow normal R rules:
#'
#' logical -> integer -> double -> complex -> character
#'
#' For each row, the set of seen values starts as logical. If the value in a new column requires
#' promotion, all values in the set are promoted accordingly. This continues iteratively for each
#' column in the row. See the examples.
#'
#' @examples
#'
#' # after processing the 2nd column, all values will be promoted to character
#' row_duplicated(data.frame(TRUE, "TRUE", TRUE))
#'
#' # after the 1st column the set is (TRUE)
#' # after the 2nd column, TRUE is promoted to 1, and the set is (1)
#' # after the 3rd column, 1 is promoted to "1", and the set is ("1", "FALSE")
#' # the final column is thus promoted to "TRUE"
#' row_duplicated(data.frame(TRUE, 1L, "FALSE", TRUE))
#'
row_duplicated <- function(.data, match_type = NULL, output_class, ...) {
    UseMethod("row_duplicated")
}

#' @rdname row_duplicated
#' @export
#'
row_duplicated.matrix <- function(.data, match_type = NULL, output_class, ...) {
    if (is.null(match_type)) match_type <- "NULL"
    match_type <- match.arg(match_type, c("NULL", "any", "none", "which_first", "count"))
    output_mode <- if (match_type %in% c("which_first", "count")) "integer" else "logical"

    if (missing(output_class)) {
        output_class <- if (match_type == "NULL") "matrix" else "vector"
    }

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_class = output_class,
                        output_mode = output_mode,
                        na_action = "pass",
                        ...)

    if (match_type == "NULL" && !metadata$output_class %in% c("matrix", "data.frame")) {
        stop("match_type = NULL requires a matrix or data.frame output class.")
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata, allow_cols = match_type == "NULL")

    extras <- list(
        match_type = match_type
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_duplicated, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_duplicated
#' @export
#'
row_duplicated.data.frame <- function(.data, match_type = NULL, output_class, ...) {
    if (is.null(match_type)) match_type <- "NULL"
    match_type <- match.arg(match_type, c("NULL", "any", "none", "which_first", "count"))
    output_mode <- if (match_type %in% c("which_first", "count")) "integer" else "logical"

    if (missing(output_class)) {
        output_class <- if (match_type == "NULL") "data.frame" else "vector"
    }

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_class = output_class,
                        output_mode = output_mode,
                        na_action = "pass",
                        ...)

    if (match_type == "NULL" && !metadata$output_class %in% c("matrix", "data.frame")) {
        stop("match_type = NULL requires a matrix or data.frame output class.")
    }

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata, allow_cols = match_type == "NULL")

    extras <- list(
        match_type = match_type
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_duplicated, metadata, .data, ans, extras)
    }

    ans
}
