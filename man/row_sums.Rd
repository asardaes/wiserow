% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/row_sums.R
\name{row_sums}
\alias{row_sums}
\alias{row_sums.matrix}
\alias{row_sums.data.frame}
\title{Row-wise sum}
\usage{
row_sums(.data, ...)

\method{row_sums}{matrix}(.data, ...)

\method{row_sums}{data.frame}(.data, ...)
}
\arguments{
\item{.data}{A two-dimensional data structure.}

\item{...}{Arguments for \code{\link[=row_arith]{row_arith()}} (except \code{operator}).}
}
\description{
Wrapper for \code{\link[=row_arith]{row_arith()}} with \code{operator = "+"}.
}
