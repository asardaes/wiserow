#' @importFrom methods as
#'
row_extrema_matrix <- function(.data, comp_op, ...) {
    stopifnot(typeof(.data) %in% c("logical", "integer", "double", "character"))

    metadata <- op_ctrl(input_class = "matrix",
                        input_modes = typeof(.data),
                        output_mode = typeof(.data),
                        ...)

    metadata <- validate_metadata(.data, metadata)
    cols <- if (is.null(metadata$cols)) seq_len(ncol(.data)) else metadata$cols

    if (length(cols) == 1L) {
        if (metadata$output_class == "data.frame") {
            return(as.data.frame(.data[, cols, drop = FALSE]))
        }
        else {
            return(methods::as(.data[, cols], metadata$output_class))
        }
    }

    ans <- prepare_output(.data, metadata)
    extras <- list(
        comp_op = comp_op
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_extrema, metadata, .data, ans, extras)
    }

    ans
}

#' @importFrom methods as
#'
row_extrema_df <- function(.data, comp_op, ...) {
    metadata <- op_ctrl(input_class = "data.frame",
                        input_modes = sapply(.data, typeof),
                        output_mode = "logical", # placeholder
                        ...)

    metadata <- validate_metadata(.data, metadata)

    cols <- if (is.null(metadata$cols)) seq_len(ncol(.data)) else metadata$cols
    input_modes <- metadata$input_modes
    factors <- sapply(.data, class) == "factor"
    if (metadata$factor_mode == "character" && any(factors)) {
        input_modes[factors] <- "character" # only modify local copy!
    }

    metadata$output_mode <- compute_output_mode(input_modes[cols],
                                                not_allowed = "complex",
                                                "Cannot compute maxima when complex numbers are involved.")

    if (length(cols) == 1L) {
        if (metadata$output_class == "data.frame") {
            return(.data[, cols, drop = FALSE])
        }
        else {
            return(methods::as(.data[, cols], metadata$output_class))
        }
    }

    ans <- prepare_output(.data, metadata)
    extras <- list(
        comp_op = comp_op
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_extrema, metadata, .data, ans, extras)
    }

    ans
}
