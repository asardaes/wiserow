#include "../core.h"

#include <stdexcept> // out_of_range
#include <string>

#include <Rcpp.h>

#include "DataFrameColumnCollection.h"
#include "MatrixColumnCollection.h"
#include "SurrogateColumn.h"

namespace wiserow {

std::size_t output_length(const OperationMetadata& metadata, const ColumnCollection& col_collection) {
    if (metadata.rows.ptr) {
        return metadata.rows.len;
    }
    else if (metadata.rows.is_null) {
        return col_collection.nrow();
    }
    else {
        return 0;
    }
}

// =================================================================================================

ColumnCollection ColumnCollection::coerce(const OperationMetadata& metadata, SEXP data) {
    switch(metadata.input_class) {
    case RClass::MATRIX: {
        switch(metadata.input_modes[0]) {
        case INTSXP: {
            return MatrixColumnCollection<INTSXP, int>(data, metadata.cols);
        }
        case REALSXP: {
            return MatrixColumnCollection<REALSXP, double>(data, metadata.cols);
        }
        case LGLSXP: {
            return MatrixColumnCollection<LGLSXP, int>(data, metadata.cols);
        }
        case STRSXP: {
            return MatrixColumnCollection<STRSXP, const char *>(data, metadata.cols);
        }
        case CPLXSXP: {
            return MatrixColumnCollection<CPLXSXP, std::complex<double>>(data, metadata.cols);
        }
        default: { // nocov start
            // can never happen because OperationMetadata's constructor checks this too
            Rcpp::stop("[wiserow] matrices can only contain integers, doubles, logicals, characters, or complex.");
        } // nocov end
        }
    }
    case RClass::DATAFRAME: {
        return DataFrameColumnCollection(data, metadata);
    }
    default: { // nocov start
        // can never happen because enums, wtf gcc?
        Rcpp::stop("[wiserow] this should never happen D=");
    } // nocov end
    }
}

// -------------------------------------------------------------------------------------------------

ColumnCollection::ColumnCollection(const std::size_t nrow)
    : nrow_(nrow)
{ }

std::size_t ColumnCollection::ncol() const {
    return columns_.size();
}

std::size_t ColumnCollection::nrow() const {
    return nrow_;
}

std::shared_ptr<const VariantColumn> ColumnCollection::operator[](const std::size_t j) const {
    return columns_[j];
}

const supported_col_t ColumnCollection::operator()(const std::size_t i, const std::size_t j) const {
    return (*(columns_[j]))[i];
}

// =================================================================================================

DataFrameColumnCollection::DataFrameColumnCollection(const Rcpp::DataFrame& df, const OperationMetadata& metadata)
    : ColumnCollection(df.nrow())
{
    int upper_j = 0,
        current_j = 0;

    if (metadata.cols.ptr) {
        upper_j = static_cast<int>(metadata.cols.len);
    }
    else if (metadata.cols.is_null) {
        upper_j = df.ncol();
    }

    for (int j = 0; j < upper_j; j++) {
        current_j = metadata.cols.ptr ? metadata.cols.ptr[j] - 1 : j;

        switch(metadata.input_modes[current_j]) {
        case INTSXP: {
            Rcpp::IntegerVector vec(df[current_j]);

            if (!Rf_isFactor(df[current_j]) || metadata.factor_mode == INTSXP) {
                columns_.push_back(std::make_shared<SurrogateColumn<int>>(&vec[0], vec.length()));
            }
            else {
                Rcpp::StringVector levels = vec.attr("levels");
                Rcpp::StringVector factors(vec.length());
                for (R_xlen_t i = 0; i < vec.length(); i++) {
                    if (Rcpp::IntegerVector::is_na(vec[i])) {
                        factors[i] = NA_STRING;
                    }
                    else {
                        factors[i] = levels[vec[i] - 1];
                    }
                }

                columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringVector>>(factors));
            }

            break;
        }
        case REALSXP: {
            Rcpp::NumericVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<double>>(&vec[0], vec.length()));
            break;
        }
        case LGLSXP: {
            Rcpp::LogicalVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<int>>(&vec[0], vec.length(), true));
            break;
        }
        case STRSXP: {
            Rcpp::StringVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringVector>>(vec));
            break;
        }
        case CPLXSXP: {
            Rcpp::ComplexVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexVector>>(vec));
            break;
        }
        default: { // nocov start
            // can never happen because OperationMetadata's constructor checks this too
            Rcpp::stop("[wiserow] data frames can only contain integers, doubles, logicals, characters, or complex.");
        } // nocov end
        }
    }
}


// =================================================================================================

MatrixColumnCollection<STRSXP, const char *>::MatrixColumnCollection(const Rcpp::Matrix<STRSXP>& mat,
                                                                     const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringMatrix>>(mat, j));
        }
    }
}

// -------------------------------------------------------------------------------------------------

MatrixColumnCollection<CPLXSXP, std::complex<double>>::MatrixColumnCollection(const Rcpp::Matrix<CPLXSXP>& mat,
                                                                              const surrogate_vector& cols)
    : ColumnCollection(mat.nrow())
{
    if (cols.ptr) {
        for (std::size_t i = 0; i < cols.len; i++) {
            int j = cols.ptr[i] - 1;
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j));
        }
    }
    else if (cols.is_null) {
        for (int j = 0; j < mat.ncol(); j++) {
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexMatrix>>(mat, j));
        }
    }
}

// =================================================================================================
/*
 * Accessing elements of STRSXP is based on STRING_ELT:
 * https://github.com/wch/r-source/blob/aa024e5fa456871f9825e9f257ae6872a9cb0722/src/main/memory.c#L3750
 *
 * STRING_ELT calls STDVEC_DATAPTR and casts the result to SEXP*:
 * https://github.com/wch/r-source/blob/aa024e5fa456871f9825e9f257ae6872a9cb0722/src/include/Rinternals.h#L441
 *
 * To get the final char* from SEXP*, we use CHAR(). This, along with STDVEC_DATAPTR, are only casts
 * with some arithmetic, so they should be thread safe (?).
 */

SurrogateColumn<Rcpp::StringMatrix>::SurrogateColumn(SEXP mat, const int j)
    : data_ptr_(static_cast<SEXP *>(STDVEC_DATAPTR(mat)))
    , size_(Rf_nrows(mat))
    , offset_(j * size_)
{ }

const supported_col_t SurrogateColumn<Rcpp::StringMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(boost::string_ref(CHAR(data_ptr_[id + offset_])));
}

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::StringVector>::SurrogateColumn(SEXP vec)
    : data_ptr_(static_cast<SEXP *>(STDVEC_DATAPTR(vec)))
    , size_(Rf_xlength(vec))
{ }

const supported_col_t SurrogateColumn<Rcpp::StringVector>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(boost::string_ref(CHAR(data_ptr_[id])));
}

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::ComplexMatrix>::SurrogateColumn(const Rcpp::ComplexMatrix& mat, const int j)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&mat[j * mat.nrow()]))
    , size_(mat.nrow())
{ }

const supported_col_t SurrogateColumn<Rcpp::ComplexMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(data_ptr_[id]);
}

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::ComplexVector>::SurrogateColumn(const Rcpp::ComplexVector& vec)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&vec[0]))
    , size_(vec.length())
{ }

const supported_col_t SurrogateColumn<Rcpp::ComplexVector>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("[wiserow] column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(data_ptr_[id]);
}

// =================================================================================================

int get_int(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<int>(metadata[key]);
}

std::string get_string(const Rcpp::List& metadata, const std::string& key) {
    return Rcpp::as<std::string>(metadata[key]);
}

RClass parse_input_class(const Rcpp::List& metadata) {
    std::string str = get_string(metadata, "input_class");

    if (str == "matrix") {
        return RClass::MATRIX;
    }
    else if (str == "data.frame") {
        return RClass::DATAFRAME;
    }
    else {
        Rcpp::stop("[wiserow] unsupported input class: " + str);
    }
}

RClass parse_output_class(const Rcpp::List& metadata) {
    std::string str = get_string(metadata, "output_class");

    if (str == "vector") {
        return RClass::VECTOR;
    }
    else if (str == "list") {
        return RClass::LIST;
    }
    else if (str == "data.frame") {
        return RClass::DATAFRAME;
    }
    else if (str == "matrix") {
        return RClass::MATRIX;
    }
    else {
        Rcpp::stop("[wiserow] unsupported output class: " + str);
    }
}

R_vec_t parse_mode(const std::string& mode_str) {
    if (mode_str == "integer") {
        return INTSXP;
    }
    else if (mode_str == "double") {
        return REALSXP;
    }
    else if (mode_str == "logical") {
        return LGLSXP;
    }
    else if (mode_str == "character") {
        return STRSXP;
    }
    else if (mode_str == "complex") {
        return CPLXSXP;
    }
    else {
        Rcpp::stop("[wiserow] unsupported mode: " + mode_str);
    }
}

std::vector<R_vec_t> parse_modes(const Rcpp::StringVector& in_modes) {
    std::vector<R_vec_t> input_modes;

    for (R_xlen_t i = 0; i < in_modes.length(); i++) {
        std::string in_mode = Rcpp::as<std::string>(in_modes(i));
        input_modes.push_back(parse_mode(in_mode));
    }

    return input_modes;
}

NaAction parse_na_action(const Rcpp::List& metadata) {
    std::string str = get_string(metadata, "na_action");

    if (str == "pass") {
        return NaAction::PASS;
    }
    else {
        return NaAction::EXCLUDE;
    }
}

surrogate_vector coerce_subset_indices(SEXP ids) {
    if (Rf_isNull(ids)) {
        return surrogate_vector(nullptr, 0, true);
    }
    else {
        Rcpp::IntegerVector vec(ids);

        if (vec.length() > 0) {
            return surrogate_vector(&vec[0], vec.length(), false);
        }
        else {
            return surrogate_vector(nullptr, 0, false);
        }
    }
}

// -------------------------------------------------------------------------------------------------

OperationMetadata::OperationMetadata(const Rcpp::List& metadata)
    : num_workers(get_int(metadata, "num_workers"))
    , input_class(parse_input_class(metadata))
    , input_modes(parse_modes(metadata["input_modes"]))
    , output_class(parse_output_class(metadata))
    , output_mode(parse_mode(get_string(metadata, "output_mode")))
    , na_action(parse_na_action(metadata))
    , cols(coerce_subset_indices(metadata["cols"]))
    , rows(coerce_subset_indices(metadata["rows"]))
    , factor_mode(parse_mode(get_string(metadata, "factor_mode")))
{ }

// =================================================================================================

ParallelWorker::ParallelWorker(const OperationMetadata& metadata, const ColumnCollection& cc)
    : metadata(metadata)
    , col_collection_(cc)
    , interrupt_grain_(interrupt_grain(this->num_ops() / metadata.num_workers, 1000, 10000))
{ }

std::size_t ParallelWorker::num_ops() const {
    if (metadata.rows.ptr) {
        return metadata.rows.len;
    }
    else if (metadata.rows.is_null) {
        return col_collection_.nrow();
    }
    else {
        return 0;
    }
}

void ParallelWorker::operator()(std::size_t begin, std::size_t end) {
    if (eptr) return;

    try {
        thread_local_ptr t_local(nullptr);

        for (std::size_t id = begin; id < end; id++) {
            if (eptr || is_interrupted(id)) break;

            t_local = work_row(corresponding_row(id), id, t_local);
        }
    }
    catch(...) {
        mutex_.lock();
        if (!eptr) eptr = std::current_exception();
        mutex_.unlock();
    }

    // make sure this is called at least once per thread call
    RcppThread::isInterrupted();
}

std::size_t ParallelWorker::corresponding_row(std::size_t id) const {
    return metadata.rows.ptr ? metadata.rows.ptr[id] - 1 : id;
}

bool ParallelWorker::is_interrupted(const std::size_t i) const {
    return RcppThread::isInterrupted(i % interrupt_grain_ == 0);
}

// how often to check for user interrupt inside a thread
int ParallelWorker::interrupt_grain(const int interrupt_check_grain, const int min, const int max) const {
    int result = interrupt_check_grain / 10;
    if (result < min) return min;
    if (result > max) return max;
    if (result < 1) return 1;
    return result;
}

} // namespace wiserow
