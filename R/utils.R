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
