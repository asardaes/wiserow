context("  Row min")

test_that("row_min works for matrices.", {
    matrices <- list(bool_na_mat, int_na_mat, dbl_na_mat, char_na_mat)
    na_actions <- wiserow:::.supported_na_actions
    out_classes <- wiserow:::.supported_output_classes

    for (mat in matrices) {
        for (na_action in na_actions) {
            base <- suppressWarnings(apply(mat, 1L, min, na.rm = na_action == "exclude"))

            if (typeof(mat) == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (typeof(mat) == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }

            for (out_class in out_classes) {
                ans <- row_min(mat, na_action = na_action, output_class = out_class)
                expected <- if (out_class == "data.frame") data.frame(V1 = base, stringsAsFactors = FALSE) else as(base, out_class)
                expect_equal(ans, expected)
            }
        }
    }
})

# no char tests, too cumbersome
test_that("row_min works for data frames.", {
    df_cols <- list(
        paste0("bool.V", 1:3),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3)),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3), paste0("dbl.V", 1:3))
    )

    na_actions <- wiserow:::.supported_na_actions
    out_classes <- wiserow:::.supported_output_classes

    for (cols in df_cols) {
        for (na_action in na_actions) {
            common_mode <- wiserow:::compute_output_mode(sapply(df[cols], typeof))
            base <- suppressWarnings(apply(df[cols], 1L, min, na.rm = na_action == "exclude"))

            if (common_mode == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (common_mode == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }

            for (out_class in out_classes) {
                ans <- row_min(df, cols = cols, na_action = na_action, output_class = out_class)
                expected <- if (out_class == "data.frame") data.frame(V1 = base, stringsAsFactors = FALSE) else as(base, out_class)
                expect_equal(ans, expected)
            }
        }
    }
})

test_that("row_min(..., which != NULL) works for matrices.", {
    matrices <- list(bool_na_mat, int_na_mat, dbl_na_mat, char_na_mat)
    na_actions <- wiserow:::.supported_na_actions
    whichs <- c("first", "last")
    out_classes <- wiserow:::.supported_output_classes

    which_min <- function(i) {
        b <- base[i]
        if (!is.na(b) && !is.infinite(b)) {
            flags <- mat[i,] == b

            if (which == "first") {
                which.max(flags)
            }
            else {
                ncol(mat) - which.max(rev(flags)) + 1L
            }
        }
        else {
            NA_integer_
        }
    }

    for (mat in matrices) {
        for (na_action in na_actions) {
            base <- suppressWarnings(apply(mat, 1L, min, na.rm = na_action == "exclude"))

            if (typeof(mat) == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (typeof(mat) == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }

            for (which in whichs) {
                intermediate <- sapply(seq_along(base), which_min)

                for (out_class in out_classes) {
                    ans <- row_min(mat, which = which, na_action = na_action, output_class = out_class)
                    expected <- if (out_class == "data.frame") data.frame(V1 = intermediate, stringsAsFactors = FALSE) else as(intermediate, out_class)
                    expect_equal(ans, expected)
                }
            }
        }
    }
})

test_that("row_min(..., which != NULL) works for data frames.", {
    df_cols <- list(
        paste0("bool.V", 1:3),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3)),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3), paste0("dbl.V", 1:3))
    )

    na_actions <- wiserow:::.supported_na_actions
    whichs <- c("first", "last")
    out_classes <- wiserow:::.supported_output_classes

    which_min <- function(i) {
        b <- base[i]
        if (!is.na(b) && !is.infinite(b)) {
            flags <- unlist(df[i, cols]) == b

            if (which == "first") {
                which.max(flags)
            }
            else {
                length(cols) - which.max(rev(flags)) + 1L
            }
        }
        else {
            NA_integer_
        }
    }

    for (cols in df_cols) {
        for (na_action in na_actions) {
            common_mode <- wiserow:::compute_output_mode(sapply(df[cols], typeof))
            base <- suppressWarnings(apply(df[cols], 1L, min, na.rm = na_action == "exclude"))

            if (common_mode == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (common_mode == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }

            for (which in whichs) {
                intermediate <- unname(sapply(seq_along(base), which_min))

                for (out_class in out_classes) {
                    ans <- row_min(df, which = which, cols = cols, na_action = na_action, output_class = out_class)
                    expected <- if (out_class == "data.frame") data.frame(V1 = intermediate, stringsAsFactors = FALSE) else as(intermediate, out_class)
                    expect_equal(ans, expected)
                }
            }
        }
    }
})
