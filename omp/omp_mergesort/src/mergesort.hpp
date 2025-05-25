// Copyright 2023 Jeisson Hidalgo jeisson.hidalgo@ucr.ac.cr CC-BY-4
#include <omp.h>

#include <algorithm>
#include <vector>

// TODO(you): parallelize merge sort
template <typename Type>
void mergesort(std::vector<Type>& values, const ptrdiff_t left,
    const ptrdiff_t right) {
  // ptrdiff_t you can have negative numbers
  // Count of elements we have to sort
  const ptrdiff_t count = right - left;
  if (count > 1024) {
    const size_t mid = (left + right) / 2;
    // Sort left half of the array
    // task genera las tareas en una cola
    // #pragma omp taskgroup
    // {
      #pragma omp task untied if (mid - left >= 5000) \
        default(none)shared(values) firstprivate(left, mid)
      mergesort(values, left, mid);
      // Sort right half of the array
      #pragma omp task untied if (right - left >= 5000) \
        default(none) shared(values) firstprivate(mid, right)
      mergesort(values, mid + 1, right);
      // Wait util both sub_vectores are sorted
      // Wait for the task that are necesary for him
      #pragma omp taskwait
      // #pragma omp taskyield
    // }
    // Both halves are sorted, merge them into a temp vector
    std::vector<Type> temp;
    temp.reserve(count + 1);
    std::merge(values.begin() + left, values.begin() + mid + 1,
        values.begin() + mid + 1, values.begin() + right + 1,
        std::back_inserter(temp));
    // Copy the sorted temp vector back to the original vector
    std::copy(temp.begin(), temp.end(), values.begin() + left);
  } else {
    std::sort(values.begin() + left, values.begin() + right + 1);
  }
}

template <typename Type>
void mergesort(std::vector<Type>& values,
    int thread_count = omp_get_max_threads()) {
  #pragma omp parallel num_threads(thread_count) default(none) shared(values)
  #pragma omp single
  mergesort(values, 0, values.size() - 1);
  // al llegar al join con task, se genra un sonsume loop
  // lo cual genera que los ilos que ya terminaron comienzen a sacar
  // de la cola las tareas que hacen falta
}
