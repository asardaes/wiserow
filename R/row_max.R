#' Row-wise maxima
#'
#' @export
#'
#' @template data-param
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
