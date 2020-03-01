if (!identical(Sys.getenv("NOT_CRAN"), "true")) {
    # see https://github.com/RcppCore/Rcpp/issues/972
    # what is mentioned there happened on r-devel-fedora-clang
    expect_error <- function(...) {
        e <- rlang::enexprs(...)

        if (is.null(e$regexp)) {
            testthat::expect_error(...)
        }
        else {
            e$regexp <- glue::glue("[unknown reason|{ e$regexp }]")
            eval(rlang::call2("expect_error", .ns = "testthat", !!!e))
        }
    }
}
