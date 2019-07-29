validate_dim <- function(.data) {
    .dim <- dim(.data)

    if (is.null(.dim) || length(.dim) != 2L) {
        stop("Invalid dimensions.")
    }
}

#' @importFrom RcppParallel defaultNumThreads
#'
num_workers <- function() {
    as.integer(Sys.getenv("RCPP_PARALLEL_NUM_THREADS", RcppParallel::defaultNumThreads()))
}
