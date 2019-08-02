#' Operation's control parameters
#'
#' @export
#'
#' @param data_class See [base::class()].
#' @param output_mode Desired [base::storage.mode()] for the result.
#' @param na_action One of "exclude" or "pass".
#' @param ... Internal.
#'
op_ctrl <- function(data_class, output_mode, na_action = "exclude", ...) {
    input_class <- match.arg(data_class, .supported_classes)
    output_mode <- match.arg(output_mode, .supported_modes)
    na_action <- match.arg(na_action, .supported_na_actions)

    list(
        ...,
        input_class = data_class,
        output_mode = output_mode,
        num_workers = num_workers(),
        na_action = na_action
    )
}
