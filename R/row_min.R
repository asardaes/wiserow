#' Row-wise minima
#'
#' @export
#'
#' @template data-param
#' @param which If not `NULL`, one of ("first", "last") to return the index where minima occurs.
#' @inheritDotParams op_ctrl -output_mode
#'
#' @details
#'
#' For data frames, if the input columns have different modes, all of them will be promoted
#' on-the-fly to the highest one in the R hierarchy.
#'
#' @note
#'
#' String comparison is done in C++, which can vary from what R does.
#'
#' @examples
#'
#' df <- data.frame(FALSE, TRUE, NA_character_)
#'
#' # all columns promoted to character (on-the-fly)
#' row_min(df)
#' row_min(df, na_action = "pass")
#'
#' # no promotion
#' row_min(df, cols = 1:2)
#'
#' row_min(df, cols = 1:2, which = "last")
#'
row_min <- function(.data, which = NULL, ...) {
    UseMethod("row_min")
}

#' @rdname row_min
#' @export
#'
row_min.matrix <- function(.data, which = NULL, ...) {
    row_extrema_matrix(.data, "<", which, ...)
}

#' @rdname row_min
#' @export
#'
row_min.data.frame <- function(.data, which = NULL, ...) {
    row_extrema_df(.data, "<", which, ...)
}
