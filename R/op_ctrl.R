#' Operation's control parameters
#'
#' @export
#' @importFrom tidyselect scoped_vars
#'
#' @param output_mode Desired [base::storage.mode()] for the result.
#' @param output_class One of ("vector", "list", "data.frame", "matrix"), possibly abbreviated.
#' @param na_action One of ("exclude", "pass"), possibly abbreviated. See [stats::na.pass] for
#'   semantics.
#' @param cols A vector indicating which columns to consider for the operation. If `NULL`, all
#'   columns are used. If its length is 0, no columns are considered. Negative numbers, logical
#'   values, character vectors representing column names, and [tidyselect::select_helpers] are
#'   supported.
#' @param rows Like `cols` but for row indices, and without `tidyselect` support.
#' @param factor_mode One of ("character", "integer"), possibly abbreviated. If a column is a
#'   factor, this determines whether the operation uses its internal integer values, or the
#'   character values from its levels.
#' @param ... Internal.
#'
#' @details
#'
#' Each function in this package supports arguments for `op_ctrl` through its ellipsis, but
#' sometimes they don't pay attention to certain options; each function specifies what can be used.
#'
#' Subsetting with `rows` or `cols` does *not* incur a deep copy of the data, only the vector with
#' desired indices is kept in memory, and look-ups are done. Support for `tidyselect` only works
#' when this function is *not* called directly.
#'
#' When a function supports `output_mode`, the result is essentially cast to the desired mode, as if
#' something like `as.logical`, `as.integer`, or similar was used; currently only supported by
#' [row_means()].
#'
#' @note
#'
#' Abbreviations are supported in accordance to the rules from [base::match.arg()].
#'
op_ctrl <- function(output_mode,
                    output_class = "vector",
                    na_action = "exclude",
                    cols = NULL,
                    rows = NULL,
                    factor_mode = "character",
                    ...)
{
    output_mode <- match.arg(output_mode, .supported_modes)
    output_class <- match.arg(output_class, .supported_output_classes)
    na_action <- match.arg(na_action, .supported_na_actions)
    factor_mode <- match.arg(factor_mode, c("integer", "character"))

    .data <- parent.frame()$.data
    if (!is.null(.data)) {
        col_names <- colnames(.data)
        do.call(tidyselect::scoped_vars, list(col_names), envir = parent.frame())
    }

    list(
        ...,
        output_class = output_class,
        output_mode = output_mode,
        num_workers = num_workers(),
        na_action = na_action,
        cols = cols,
        rows = rows,
        factor_mode = factor_mode
    )
}
