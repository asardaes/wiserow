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
            return(as(.data[, cols], metadata$output_class))
        }
    }

    ans_len <- if (is.null(metadata$rows)) nrow(.data) else length(metadata$rows)
    if (metadata$output_mode == "double") {
        ans <- rep(-Inf, ans_len)
    }
    else {
        ans <- rep(as(NA, metadata$output_mode), ans_len)
    }

    ans <- if (metadata$output_class == "data.frame") data.frame(V1 = ans, stringsAsFactors = FALSE) else as(ans, metadata$output_class)
    extras <- list(
        comp_op = comp_op
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_extrema, metadata, .data, ans, extras)
    }

    ans
}

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
            return(as(.data[, cols], metadata$output_class))
        }
    }

    ans_len <- if (is.null(metadata$rows)) nrow(.data) else length(metadata$rows)
    if (metadata$output_mode == "double") {
        ans <- rep(-Inf, ans_len)
    }
    else {
        ans <- rep(as(NA, metadata$output_mode), ans_len)
    }

    ans <- if (metadata$output_class == "data.frame") data.frame(V1 = ans, stringsAsFactors = FALSE) else as(ans, metadata$output_class)
    extras <- list(
        comp_op = comp_op
    )

    if (NROW(ans) > 0L) {
        .Call(C_row_extrema, metadata, .data, ans, extras)
    }

    ans
}
