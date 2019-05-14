// Requires Google Bench which is not setup as part of this project yet.

/*
	This is just a dump after I decided to play around with the StringCompareIgnoreCase function (used in x86_data_impl.cpp)
	I managed to get it down to roughly the same speed as a normal strcmp (that is the StringCompareIgnoreCase_Optimised) version.
		
		StringCompareIgnoreCase_Original : Is the function as I wrote it originally (after deciding that I needed a case insensitive strcmp)
		StringCompareIgnoreCase_Naive    : Never actually been used, but I wrote it as something that _could_
                                           have been written, and used it just as a frame of reference.
		StringCompareIgnoreCase_Optimised : The fastest way of comparing a string (conforming to strcmp's API) I have worked out so far.

		StringCompare_Strcmp : A reference benchmark -> This is what it was before when just using strmp (when I didn't need case insensitivity).
	*/

#include <cstring>

static int StringCompareIgnoreCase_Optimized(const char* a, const char* b) {
  assert(a != nullptr);
  assert(b != nullptr);

  if (a[0] == 0) return -1;

  if (b[0] == 0) return 1;

  const std::size_t len = std::strlen(a);
  for (std::size_t i = 0; i < len; ++i) {
    unsigned char ca = static_cast<unsigned char>(a[i]);
    unsigned char cb = static_cast<unsigned char>(b[i]);

    if (ca >= 'A' && ca <= 'Z') ca |= 0x20;

    if (cb >= 'A' && cb <= 'Z') cb |= 0x20;

    if (ca != cb) return ca - cb;
  }

  return 0;
}

static int StringCompareIgnoreCase_Original(const char* a, const char* b) {
  assert(a != nullptr);
  assert(b != nullptr);

  const std::size_t a_len = std::strlen(a);
  const std::size_t b_len = std::strlen(b);

  if (a_len == 0) return -1;

  if (b_len == 0) return 1;

  for (std::size_t i = 0; i < a_len; ++i) {
    const unsigned char ca = std::tolower(static_cast<unsigned char>(a[i]));
    const unsigned char cb = std::tolower(static_cast<unsigned char>(b[i]));

    if (ca != cb) return ca < cb ? -1 : 1;
  }

  return 0;
}

static int StringCompareIgnoreCase_Naive(const char* a, const char* b) {
  std::string ac(a);
  std::string bc(b);

  for (char& c : ac) c = std::tolower((unsigned char)c);

  for (char& c : bc) c = std::tolower((unsigned char)c);

  return std::strcmp(ac.c_str(), bc.c_str());
}

static void StringCompare_Original(benchmark::State& state) {
  const char* a = "Hello";
  const char* b = "World";

  for (auto _ : state) {
    int x = StringCompareIgnoreCase_Original(a, b);
    benchmark::DoNotOptimize(x);
  }
}
BENCHMARK(StringCompare_Original);

static void StringCompare_Optimized(benchmark::State& state) {
  const char* a = "Hello";
  const char* b = "World";
  for (auto _ : state) {
    int x = StringCompareIgnoreCase_Optimized(a, b);
    benchmark::DoNotOptimize(x);
  }
}
BENCHMARK(StringCompare_Optimized);

static void StringCompare_Naive(benchmark::State& state) {
  const char* a = "Hello";
  const char* b = "World";
  for (auto _ : state) {
    int x = StringCompareIgnoreCase_Naive(a, b);
    benchmark::DoNotOptimize(x);
  }
}
BENCHMARK(StringCompare_Naive);

static void StringCompare_Strcmp(benchmark::State& state) {
  const char* a = "hello";
  const char* b = "world";
  for (auto _ : state) {
    int x = std::strcmp(a, b);
    benchmark::DoNotOptimize(x);
  }
}
BENCHMARK(StringCompare_Strcmp);
