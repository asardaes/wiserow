#' Operation's control parameters
#'
#' @export
#'
#' @param output_mode Desired [base::storage.mode()] for the result.
#' @param na_action One of "exclude" or "pass".
#' @param cols A vector indicating which columns to consider for the operation. If `NULL`, all
#'   columns are used. If its length is 0, no columns are considered.
#' @param ... Internal.
#'
op_ctrl <- function(output_mode, na_action = "exclude", cols = NULL, ...) {
    output_mode <- match.arg(output_mode, .supported_modes)
    na_action <- match.arg(na_action, .supported_na_actions)

    list(
        ...,
        output_mode = output_mode,
        num_workers = num_workers(),
        na_action = na_action,
        cols = cols
    )
}
