#' Row-wise maxima
#'
#' @export
#'
#' @param .data `r roxygen_data_param()`
#' @param which If not `NULL`, one of ("first", "last") to return the index where maxima occurs.
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
#' row_max(df)
#' row_max(df, na_action = "pass")
#'
#' # no promotion
#' row_max(df, cols = 1:2)
#'
#' row_max(df, cols = 1:2, which = "first")
#'
row_max <- function(.data, which = NULL, ...) {
    UseMethod("row_max")
}

#' @rdname row_max
#' @export
#'
row_max.matrix <- function(.data, which = NULL, ...) {
    row_extrema_matrix(.data, ">", which, ...)
}

#' @rdname row_max
#' @export
#'
row_max.data.frame <- function(.data, which = NULL, ...) {
    row_extrema_df(.data, ">", which, ...)
}
