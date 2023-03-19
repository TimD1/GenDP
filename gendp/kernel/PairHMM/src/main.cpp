#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

#ifndef DENORMALS
#include <xmmintrin.h>
#endif

#include "input_reader.h"
#include "testcase_iterator.h"
#include "testcase.h"
#include "pairhmm.h"
#include "pairhmm_scalarimpl.h"
#include "pairhmm_vecimpl.h"
#include "pairhmm_sseimpl.h"
#include "pairhmm_avximpl.h"
#include "chronos.h"

using namespace std;

int main (const int argc, char const * const argv[]) {
#ifndef DENORMALS
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif
  auto pairhmm = Pairhmm<
    PairhmmScalarImpl<float>,
    PairhmmScalarImpl<double>
    //PairhmmSSEFloatImpl,
    // PairhmmAVXFloatImpl,
    // PairhmmAVXDoubleImpl
    // PairhmmAVXFloat2DiagsImpl,
    // PairhmmAVXDouble2DiagsImpl
  >{};
  InputReader<TestcaseIterator> reader {};
  // if (argc == 2)
  reader.from_file(argv[1]);
  double computation_time = 0.f;
  Chronos time;
  int index = 0;
  int input_size = atoi(argv[2]);
  for (auto& testcase : reader) {
    index++;
    // printf("%d\n", index);
    time.reset();
    auto results = pairhmm.calculate(testcase);
    computation_time += time.elapsed();
    if (input_size > 0 && input_size == index) break;
    // for (auto x : results)
    //   cout << x << "\n";
    // cout << "\n";
  }
  std::cerr << "done in " << computation_time << "ms\n";
  return 0;
}
