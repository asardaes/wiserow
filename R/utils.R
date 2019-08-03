.supported_output_classes <- c("vector")

.supported_modes <- c("integer",
                      "double",
                      "logical",
                      "character",
                      "complex")

.supported_na_actions <- c("exclude",
                           "pass")

#' @importFrom RcppParallel defaultNumThreads
#'
num_workers <- function() {
    as.integer(Sys.getenv("RCPP_PARALLEL_NUM_THREADS", RcppParallel::defaultNumThreads()))
}

#' @importFrom glue glue
#'
validate_metadata <- function(.data, metadata) {
    if (typeof(metadata$cols) == "double") {
        metadata$cols <- as.integer(metadata$cols)
    }

    if (typeof(metadata$rows) == "double") {
        metadata$rows <- as.integer(metadata$rows)
    }

    nc <- ncol(.data)
    nr <- nrow(.data)

    if (any(metadata$cols > nc)) {
        stop(glue::glue("Invalid column indices, data has {nc} columns, received: ",
                        "[{paste(metadata$cols, collapse = ',')}]"))
    }
    else if (any(metadata$cols < 1L)) {
        metadata$cols <- setdiff(1L:nc, -1L * metadata$cols)
    }

    if (any(metadata$rows > nr)) {
        stop(glue::glue("Invalid row indices, data has {nr} rows, received: ",
                        "[{paste(metadata$rows, collapse = ',')}]"))
    }
    else if (any(metadata$rows < 1L)) {
        metadata$rows <- setdiff(1L:nr, -1L * metadata$rows)
    }

    metadata
}

prepare_output <- function(.data, metadata) {
    ans_len <- if (is.null(metadata$rows)) nrow(.data) else length(metadata$rows)
    vector(metadata$output_mode, ans_len)
}
