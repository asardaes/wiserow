context("  Row max")

test_that("row_max works for matrices.", {
    matrices <- list(bool_na_mat, int_na_mat, dbl_na_mat, char_na_mat)
    na_actions <- wiserow:::.supported_na_actions
    out_classes <- wiserow:::.supported_output_classes

    for (mat in matrices) {
        for (na_action in na_actions) {
            base <- suppressWarnings(apply(mat, 1L, max, na.rm = na_action == "exclude"))

            if (typeof(mat) == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (typeof(mat) == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }

            for (out_class in out_classes) {
                ans <- row_max(mat, na_action = na_action, output_class = out_class)
                expected <- if (out_class == "data.frame") data.frame(V1 = base, stringsAsFactors = FALSE) else as(base, out_class)
                expect_equal(ans, expected)
            }
        }
    }
})

test_that("row_max works for data frames.", {
    df_cols <- list(
        paste0("bool.V", 1:3),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3)),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3), paste0("dbl.V", 1:3)),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3), paste0("dbl.V", 1:3), paste0("char.V", 1:3))
    )

    na_actions <- wiserow:::.supported_na_actions
    out_classes <- wiserow:::.supported_output_classes

    max_df <- function(df, cols, na.rm) {
        sapply(seq_len(nrow(df)), function(i) {
            x <- unlist(df[i, cols])
            suppressWarnings(max(x, na.rm = na.rm))
        })
    }

    for (cols in df_cols) {
        for (na_action in na_actions) {
            common_mode <- wiserow:::compute_output_mode(sapply(df[cols], typeof))

            if (common_mode != "character") {
                base <- suppressWarnings(apply(df[cols], 1L, max, na.rm = na_action == "exclude"))
            }

            if (common_mode == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (common_mode == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }
            else if (common_mode == "character") {
                base <- max_df(df, cols, na_action == "exclude")

                # C++ string comparison says that any letter is greater than "TRUE" or "FALSE" or numbers
                char_cols <- paste0("char.V", 1:3)

                invalid <- sapply(base, function(x) { !is.na(x) & !(x %in% letters) }) &
                    apply(df[char_cols], 1L, function(x) { any(!is.na(x)) })

                if (any(invalid)) {
                    base[invalid] <- apply(df[unname(invalid), char_cols, drop = FALSE], 1L, max, na.rm = na_action == "exclude")
                }
            }

            for (out_class in out_classes) {
                ans <- row_max(df, cols = cols, na_action = na_action, output_class = out_class)
                expected <- if (out_class == "data.frame") data.frame(V1 = base, stringsAsFactors = FALSE) else as(base, out_class)
                expect_equal(ans, expected)
            }
        }
    }
})

test_that("Type promotion works when `which` is not NULL.", {
    df <- data.frame(
        a = 0L,
        b = 1.0,
        c = FALSE,
        d = "a",
        e = "a"
    )

    expect_identical(row_max(df, which = "first"), 4L)
    expect_identical(row_max(df, which = "last"), 5L)

    df$d <- FALSE
    df$e <- NA_character_

    expect_identical(row_max(df), "FALSE")
    expect_identical(row_max(df, which = "first"), 3L)
    expect_identical(row_max(df, which = "last"), 4L)
})

test_that("row_max(..., which != NULL) works for matrices.", {
    matrices <- list(bool_na_mat, int_na_mat, dbl_na_mat, char_na_mat)
    na_actions <- wiserow:::.supported_na_actions
    whichs <- c("first", "last")
    out_classes <- wiserow:::.supported_output_classes

    which_max <- function(i) {
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
            base <- suppressWarnings(apply(mat, 1L, max, na.rm = na_action == "exclude"))

            if (typeof(mat) == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (typeof(mat) == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }

            for (which in whichs) {
                intermediate <- sapply(seq_along(base), which_max)

                for (out_class in out_classes) {
                    ans <- row_max(mat, which = which, na_action = na_action, output_class = out_class)
                    expected <- if (out_class == "data.frame") data.frame(V1 = intermediate, stringsAsFactors = FALSE) else as(intermediate, out_class)
                    expect_equal(ans, expected)
                }
            }
        }
    }
})

test_that("row_max(..., which != NULL) works for data frames.", {
    df_cols <- list(
        paste0("bool.V", 1:3),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3)),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3), paste0("dbl.V", 1:3)),
        c(paste0("bool.V", 1:3), paste0("int.V", 1:3), paste0("dbl.V", 1:3), paste0("char.V", 1:3))
    )

    na_actions <- wiserow:::.supported_na_actions
    whichs <- c("first", "last")
    out_classes <- wiserow:::.supported_output_classes

    max_df <- function(df, cols, na.rm) {
        sapply(seq_len(nrow(df)), function(i) {
            x <- unlist(df[i, cols])
            suppressWarnings(max(x, na.rm = na.rm))
        })
    }

    which_max <- function(i) {
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

            if (common_mode != "character") {
                base <- suppressWarnings(apply(df[cols], 1L, max, na.rm = na_action == "exclude"))
            }

            if (common_mode == "logical") {
                base[is.infinite(base)] <- NA_real_
                base <- as.logical(base)
            }
            else if (common_mode == "integer") {
                base[is.infinite(base)] <- NA_real_
                base <- as.integer(base)
            }
            else if (common_mode == "character") {
                base <- max_df(df, cols, na_action == "exclude")

                # C++ string comparison says that any letter is greater than "TRUE" or "FALSE" or numbers
                char_cols <- paste0("char.V", 1:3)

                invalid <- sapply(base, function(x) { !is.na(x) & !(x %in% letters) }) &
                    apply(df[char_cols], 1L, function(x) { any(!is.na(x)) })

                if (any(invalid)) {
                    base[invalid] <- apply(df[unname(invalid), char_cols, drop = FALSE], 1L, max, na.rm = na_action == "exclude")
                }
            }

            for (which in whichs) {
                intermediate <- unname(sapply(seq_along(base), which_max))

                for (out_class in out_classes) {
                    ans <- row_max(df, which = which, cols = cols, na_action = na_action, output_class = out_class)
                    expected <- if (out_class == "data.frame") data.frame(V1 = intermediate, stringsAsFactors = FALSE) else as(intermediate, out_class)
                    expect_equal(ans, expected)
                }
            }
        }
    }
})
