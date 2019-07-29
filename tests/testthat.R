library(wiserow)
library(testthat)

# old reporter for CMD checks
options(testthat.default_reporter = "summary")

#' To test in a local machine:
#' Sys.setenv(NOT_CRAN = "true"); test_dir("tests/testthat/")
#' OR
#' devtools::test()
#'
testthat::test_check("wiserow")
