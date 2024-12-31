test_that("row_infs works as expected.", {
    dbl_cols <- sapply(df, is.double)
    cplx_cols <- sapply(df, is.complex)

    df[, dbl_cols] <- lapply(df[, dbl_cols], function(x) { replace(x, is.na(x), Inf) })
    df[, cplx_cols] <- lapply(df[, cplx_cols], function(x) { replace(x, is.na(x), as.complex(Inf)) })

    expected <- sapply(3001:5000, df = df, function(i, df) { all(sapply(df[i, , drop = FALSE], is.infinite)) })
    ans <- row_infs(df, "all", rows = 3001:5000)
    expect_identical(ans, expected)
    ans <- row_infs(df, "all", rows = 3001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(3001:5000, df = df, function(i, df) { all(!sapply(df[i, , drop = FALSE], is.infinite)) })
    ans <- row_infs(df, "none", rows = 3001:5000)
    expect_identical(ans, expected)
    ans <- row_infs(df, "none", rows = 3001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(3001:5000, df = df, function(i, df) { any(sapply(df[i, , drop = FALSE], is.infinite)) })
    ans <- row_infs(df, "any", rows = 3001:5000)
    expect_identical(ans, expected)
    ans <- row_infs(df, "any", rows = 3001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(3001:5000, df = df, function(i, df) { Position(is.infinite, df[i, , drop = FALSE]) })
    ans <- row_infs(df, "which_first", rows = 3001:5000)
    expect_identical(ans, expected)

    expect_true(all(row_infs(int_na_mat, "none")))
    expect_true(all(row_infs(dbl_na_mat, "none")))
    expect_true(all(row_infs(bool_na_mat, "none")))
    expect_true(all(row_infs(cplx_na_mat, "none")))
})

test_that("row_infs behaves like in R for infinite/NaN values.", {
    df <- data.frame(NaN, NA_real_, NA_complex_)
    expect_true(row_infs(df, "none"))
    expect_true(row_infs(data.frame(Inf, -Inf, as.complex(Inf), as.complex(-Inf)), "all"))
})

test_that("row_infs for match_type='count' works.", {
    mat <- apply(dbl_na_mat, 2L, function(x) { replace(x, is.na(x), -Inf) })

    expected <- apply(mat, 1L, function(row) { sum(is.infinite(row)) })
    ans <- row_infs(mat, "count")
    expect_identical(ans, expected)

    dbl_cols <- sapply(df, is.double)
    cplx_cols <- sapply(df, is.complex)

    df[, dbl_cols] <- lapply(df[, dbl_cols], function(x) { replace(x, is.na(x), Inf) })
    df[, cplx_cols] <- lapply(df[, cplx_cols], function(x) { replace(x, is.na(x), as.complex(Inf)) })

    expected <- sapply(3001:5000, df = df, function(i, df) { sum(sapply(df[i, , drop = FALSE], is.infinite)) })
    ans <- row_infs(df, "count", rows = 3001:5000)
    expect_identical(ans, expected)
})
