#include "SurrogateColumn.h"

namespace wiserow {

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::StringMatrix>::SurrogateColumn(const Rcpp::StringMatrix& mat, const int j, const std::size_t size)
    : data_ptr_(size)
    , size_(size)
{
    for (std::size_t i = 0; i < size; i++) {
        // https://stackoverflow.com/a/7875438/5793905
        Rcpp::CharacterVector one_string = Rcpp::as<Rcpp::CharacterVector>(mat[i + j * size]);
        data_ptr_[i] = (char *)(one_string[0]);
    }
}

const supported_col_t SurrogateColumn<Rcpp::StringMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("Column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(boost::string_ref(data_ptr_[id]));
}

// -------------------------------------------------------------------------------------------------

SurrogateColumn<Rcpp::ComplexMatrix>::SurrogateColumn(const Rcpp::ComplexMatrix& mat, const int j, const std::size_t size)
    : data_ptr_(reinterpret_cast<const std::complex<double> *>(&mat[j * size]))
    , size_(size)
{ }

const supported_col_t SurrogateColumn<Rcpp::ComplexMatrix>::operator[](const std::size_t id) const {
    if (id >= size_) { // nocov start
        throw std::out_of_range("Column of size " +
                                std::to_string(size_) +
                                " cannot be indexed at " +
                                std::to_string(id));
    } // nocov end

    return supported_col_t(data_ptr_[id]);
}

} // namespace wiserow
