#' Conditions related to finite values
#'
#' @export
#' @templateVar par match_type
#' @templateVar choices ("all", "any", "none", "which_first", "count")
#'
#' @template data-param
#' @template generic-choices
#' @inheritDotParams op_ctrl -output_mode -na_action -factor_mode
#'
#' @examples
#'
#' # consistency with R with respect to NA/NaN
#' df <- data.frame(NaN, NA_real_, NA_complex_, NA_integer_, NA_character_, NA)
#' sapply(df, is.finite)
#' row_finites(df, "none")
#'
#' # complex also supported
#' mat <- matrix(as.complex(Inf), nrow = 2L, ncol = 2L)
#' mat[1L] <- 0+0i
#' row_finites(mat, "any")
#'
row_finites <- function(.data, match_type = "none", ...) {
    UseMethod("row_finites")
}

#' @rdname row_finites
#' @export
#'
row_finites.matrix <- function(.data, match_type = "none", ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))
    output_mode <- if (match_type %in% c("which_first", "count")) "integer" else "logical"

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = output_mode,
                        na_action = "pass",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_finites, metadata, .data, ans, extras)
    }

    ans
}

#' @rdname row_finites
#' @export
#'
row_finites.data.frame <- function(.data, match_type = "none", ...) {
    match_type <- match.arg(match_type, c("all", "any", "none", "which_first", "count"))
    output_mode <- if (match_type %in% c("which_first", "count")) "integer" else "logical"

    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_mode = output_mode,
                        na_action = "pass",
                        factor_mode = "integer",
                        ...)

    metadata <- validate_metadata(.data, metadata)
    ans <- prepare_output(.data, metadata)

    extras <- list(
        match_type = match_type
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_finites, metadata, .data, ans, extras)
    }

    ans
}
