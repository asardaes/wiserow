.supported_classes <- c("matrix")

.supported_modes <- c("integer",
                      "double",
                      "logical",
                      "character")

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

    nc <- ncol(.data)

    if (any(metadata$cols > ncol(.data))) {
        stop(glue::glue("Invalid column indices, data has {nc} columns, received: ",
                        "[{paste(metadata$cols, collapse = ',')}]"))
    }

    metadata
}
