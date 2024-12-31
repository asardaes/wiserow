test_that("row_finites works as expected.", {
    dbl_cols <- sapply(df, is.double)
    cplx_cols <- sapply(df, is.complex)

    df[2001:4000, dbl_cols] <- lapply(df[2001:4000, dbl_cols], function(x) { replace(x, is.na(x), Inf) })
    df[2001:4000, cplx_cols] <- lapply(df[2001:4000, cplx_cols], function(x) { replace(x, is.na(x), as.complex(Inf)) })

    expected <- sapply(3001:5000, df = df, function(i, df) { all(sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "all", rows = 3001:5000)
    expect_identical(ans, expected)
    ans <- row_finites(df, "all", rows = 3001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(3001:5000, df = df, function(i, df) { all(!sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "none", rows = 3001:5000)
    expect_identical(ans, expected)
    ans <- row_finites(df, "none", rows = 3001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(3001:5000, df = df, function(i, df) { any(sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "any", rows = 3001:5000)
    expect_identical(ans, expected)
    ans <- row_finites(df, "any", rows = 3001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(3001:5000, df = df, function(i, df) { Position(is.finite, df[i, , drop = FALSE]) })
    ans <- row_finites(df, "which_first", rows = 3001:5000)
    expect_identical(ans, expected)

    expect_true(all(row_finites(int_mat, "all")))
    expect_true(all(row_finites(dbl_mat, "all")))
    expect_true(all(row_finites(bool_mat, "all")))
    expect_true(all(row_finites(cplx_mat, "all")))
})

test_that("row_finites behaves like in R for NA/NaN values.", {
    df <- data.frame(NaN, NA_real_, NA_complex_, NA_integer_, NA_character_, NA)
    expect_true(row_finites(df, "none"))
})

test_that("row_finites for match_type='count' works.", {
    mat <- apply(dbl_na_mat, 2L, function(x) { replace(x, is.na(x), -Inf) })

    expected <- apply(mat, 1L, function(row) { sum(is.finite(row)) })
    ans <- row_finites(mat, "count")
    expect_identical(ans, expected)

    dbl_cols <- sapply(df, is.double)
    cplx_cols <- sapply(df, is.complex)

    df[, dbl_cols] <- lapply(df[, dbl_cols], function(x) { replace(x, is.na(x), Inf) })
    df[, cplx_cols] <- lapply(df[, cplx_cols], function(x) { replace(x, is.na(x), as.complex(Inf)) })

    expected <- sapply(3001:5000, df = df, function(i, df) { sum(sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "count", rows = 3001:5000)
    expect_identical(ans, expected)
})
