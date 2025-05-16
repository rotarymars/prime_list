#include <boost/multiprecision/cpp_int.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <algorithm>
#include <sys/resource.h>
#define _DEBUG

using boost::multiprecision::cpp_int;

bool isPrime(const cpp_int& n, const std::vector<cpp_int>& primes) {
  if (n <= 1)
    return false;
  if (n == 2 || n == 3)
    return true;
  if (n % 2 == 0 || n % 3 == 0)
    return false;
  for (const auto& prime : primes) {
    if (prime * prime > n)
      return true;
    if (n % prime == 0)
      return false;
  }
  return true;
}

int main() {
  constexpr long long PRIME_COUNT = 1000000;
  std::vector<cpp_int> primes;
  std::string path = "data/";
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  }

  std::regex number_pattern(R"((\d+)\.txt$)");
  long long filecntmax = -1;
  cpp_int primesmaxint = 2;

  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    std::string filename = entry.path().filename().string();
    std::smatch match;
    if (std::regex_search(filename, match, number_pattern)) {
      filecntmax = std::max(filecntmax, std::stoll(match[1]));
      std::ifstream infile(path + filename);
#ifdef _DEBUG
      std::cout << "Reading " << filename << std::endl;
#endif
      std::string line;
      while (std::getline(infile, line)) {
        cpp_int prime(line);
        primes.push_back(prime);
      }
    }
  }
  std::sort(primes.begin(), primes.end());
  if (!primes.empty()) {
    primesmaxint = primes.back();
    ++primesmaxint;
  }
  constexpr int operation_count = 1;
  for (int i = 0; i < operation_count; ++i) {
    std::ofstream outfile(path + std::to_string(++filecntmax) + ".txt");
    long long insertcnt = 0;

    while (insertcnt < PRIME_COUNT) {
      if (isPrime(primesmaxint, primes)) {
        outfile << primesmaxint << '\n';
        primes.push_back(primesmaxint);
        ++insertcnt;
      }
      ++primesmaxint;
    }
  }
  // Get peak memory usage (in GB)
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  std::cout << "Peak memory usage: " << (usage.ru_maxrss / 1024.0 / 1024.0) << " GB" << std::endl;
}

