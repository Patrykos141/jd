#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cstdint>
#ifdef _OPENMP
    #include <omp.h>
#endif

namespace py = pybind11;

// Enum do klas wynikowych
enum Classification : int32_t {
    FUTURE_DATE     = -2,
    SKIPPED         = -1,
    UNKNOWN_CASE    = -99,
    OK_LOW_PRIORITY = 0,
    FRESH_RETRY     = 88,
    MANUAL_HIGH     = 55,
    CRITICAL_ERROR  = 100
};

py::array_t<int32_t> classify_aging(
    py::array_t<int32_t> dates,
    py::array_t<int32_t> codes,
    int32_t date_tolerance,
    py::array_t<int32_t> priority,
    py::array_t<int32_t> retry_count) {

    auto buf = dates.request();
    auto buf_codes = codes.request();
    auto buf_priority = priority.request();
    auto buf_retry = retry_count.request();

    int32_t* ptr = static_cast<int32_t*>(buf.ptr);
    int32_t* ptr_codes = static_cast<int32_t*>(buf_codes.ptr);
    int32_t* ptr_prio = static_cast<int32_t*>(buf_priority.ptr);
    int32_t* ptr_retry = static_cast<int32_t*>(buf_retry.ptr);

    size_t n = buf.shape[0];
    py::array_t<int32_t> result(n);
    int32_t* out = result.mutable_data();

    py::gil_scoped_release release;

    #pragma omp parallel for schedule(static, 1024)
    for (long i = 0; i < static_cast<long>(n); i++) {
        int_fast32_t date  = ptr[i];
        int_fast32_t code  = ptr_codes[i];
        int_fast32_t prio  = ptr_prio[i];
        int_fast32_t retry = ptr_retry[i];
        int_fast32_t diff  = date_tolerance - date;

        bool future          = diff < 0;
        bool is_error        = (code == 2);
        bool is_retry        = (code == 4);
        bool is_skip         = (code == 3);
        bool is_manual       = (code == 5);
        bool is_ok_lowprio   = (code == 1 && prio == 0);
        bool is_critical_err = (is_error && diff > 90 && prio == 2);
        bool is_fresh_retry  = (is_retry && diff <= 7 && retry < 3);
        bool is_manual_high  = (is_manual && prio != 0);

        if (future) {
            out[i] = FUTURE_DATE;
        } else if (is_critical_err) {
            out[i] = CRITICAL_ERROR;
        } else if (is_fresh_retry) {
            out[i] = FRESH_RETRY;
        } else if (is_skip) {
            out[i] = SKIPPED;
        } else if (is_manual_high) {
            out[i] = MANUAL_HIGH;
        } else if (is_ok_lowprio) {
            out[i] = OK_LOW_PRIORITY;
        } else {
            out[i] = UNKNOWN_CASE;
        }
    }

    return result;
}

PYBIND11_MODULE(classify, m){
    m.def("calculation", &classify_aging, "Ultra szybka klasyfikacja warunkowa z OpenMP");
}
