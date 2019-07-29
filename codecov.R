library("covr")

covr_flags <- getOption("covr.flags")

options(covr.exclude_pattern = rex::rex("#" %or% "//", any_spaces, "nocov"),
        covr.exclude_start = rex::rex("#" %or% "//", any_spaces, "nocov", any_spaces, "start"),
        covr.exclude_end = rex::rex("#" %or% "//", any_spaces, "nocov", any_spaces, "end"),
        covr.flags = sapply(covr_flags, function(dummy) { "--coverage" }))

covr::codecov(type = "tests", quiet = FALSE)

#' to run locally, run all code above except codecov(...), and:
#'
#' Sys.setenv(NOT_CRAN = "true"); covr::report(package_coverage(type = "tests", quiet = FALSE))
