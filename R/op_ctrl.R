#' Operation's control parameters
#'
#' @export
#'
#' @param output_mode Desired [base::storage.mode()] for the result.
#' @param output_class One of ("vector", "list"). Possibly abbreviated.
#' @param na_action One of ("exclude", "pass"). Possibly abbreviated.
#' @param cols A vector indicating which columns to consider for the operation. If `NULL`, all
#'   columns are used. If its length is 0, no columns are considered. Negative numbers, logical
#'   values , and character vectors representing row/column names are supported.
#' @param rows Like `cols` but for row indices.
#' @param ... Internal.
#'
op_ctrl <- function(output_mode, output_class = "vector", na_action = "exclude", cols = NULL, rows = NULL, ...) {
    output_mode <- match.arg(output_mode, .supported_modes)
    output_class <- match.arg(output_class, .supported_output_classes)
    na_action <- match.arg(na_action, .supported_na_actions)

    list(
        ...,
        output_class = output_class,
        output_mode = output_mode,
        num_workers = num_workers(),
        na_action = na_action,
        cols = cols,
        rows = rows
    )
}
