% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/op_ctrl.R
\name{op_ctrl}
\alias{op_ctrl}
\title{Operation's control parameters}
\usage{
op_ctrl(
  output_mode,
  output_class = "vector",
  na_action = "exclude",
  cols = NULL,
  rows = NULL,
  factor_mode = "character",
  ...
)
}
\arguments{
\item{output_mode}{Desired \code{\link[base:mode]{base::storage.mode()}} for the result.}

\item{output_class}{One of ("vector", "list", "data.frame", "matrix"), possibly abbreviated.}

\item{na_action}{One of ("exclude", "pass"), possibly abbreviated. See \link[stats:na.fail]{stats::na.pass} for
semantics.}

\item{cols}{A vector indicating which columns to consider for the operation. If \code{NULL}, all
columns are used. If its length is 0, no columns are considered. Negative numbers, logical
values, character vectors representing column names, and \link[tidyselect:language]{tidyselect::select_helpers} are
supported.}

\item{rows}{Like \code{cols} but for row indices, and without \code{tidyselect} support.}

\item{factor_mode}{One of ("character", "integer"), possibly abbreviated. If a column is a
factor, this determines whether the operation uses its internal integer values, or the
character values from its levels.}

\item{...}{Internal.}
}
\description{
Operation's control parameters
}
\details{
Each function in this package supports arguments for \code{op_ctrl} through its ellipsis, but
sometimes they don't pay attention to certain options; each function specifies what can be used.

Subsetting with \code{rows} or \code{cols} does \emph{not} incur a deep copy of the data, only the vector with
desired indices is kept in memory, and look-ups are done. Support for \code{tidyselect} only works
when this function is \emph{not} called directly.

When a function supports \code{output_mode}, the result is essentially cast to the desired mode, as if
something like \code{as.logical}, \code{as.integer}, or similar was used; currently only supported by
\code{\link[=row_means]{row_means()}}.
}
\note{
Abbreviations are supported in accordance to the rules from \code{\link[base:match.arg]{base::match.arg()}}.
}
