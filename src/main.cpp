#include <sys/resource.h>
#define BOOST_TIMER_ENABLE_DEPRECATED
#include <algorithm>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/progress.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#define _DEBUG

using boost::multiprecision::cpp_int;

bool isPrime(const cpp_int& n, const std::vector<cpp_int>& primes) {
  if (n <= 1) return false;
  if (n == 2 || n == 3) return true;
  if (n % 2 == 0 || n % 3 == 0) return false;
  for (const auto& prime : primes) {
    if (prime * prime > n) return true;
    if (n % prime == 0) return false;
  }
  return true;
}

int main() {
  constexpr long long PRIME_COUNT = 1000000;
  std::vector<cpp_int> primes;
  const std::string path = "data/";
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  }

  std::regex number_pattern(R"((\d+)\.txt$)");
  long long filecntmax = -1;
  cpp_int primesmaxint = 2;
  auto start_time = std::chrono::high_resolution_clock::now();
  std::vector<std::string> filenames;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    std::string filename = entry.path().filename().string();
    std::smatch match;
    if (std::regex_search(filename, match, number_pattern)) {
      filenames.push_back(filename);
      filecntmax = std::max(filecntmax, std::stoll(match[1]));
    }
  }
  std::sort(filenames.begin(), filenames.end(), [&](const std::string& a, const std::string& b) {
    std::smatch match;
    std::regex_search(a, match, number_pattern);
    int a_num = std::stoi(match[1]);
    std::regex_search(b, match, number_pattern);
    int b_num = std::stoi(match[1]);
    return a_num < b_num;
  });
  for (const auto& filename : filenames) {
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
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
  std::cout << "Reading " << filecntmax << " files completed in " << duration.count() << " seconds" << std::endl;
  if (!primes.empty()) {
    primesmaxint = primes.back();
    ++primesmaxint;
  }
  constexpr int operation_count = 1;
  for (int i = 0; i < operation_count; ++i) {
    start_time = std::chrono::high_resolution_clock::now();

    std::ofstream outfile(path + std::to_string(++filecntmax) + ".txt");
    long long insertcnt = 0;
    boost::progress_display progress(PRIME_COUNT);
    while (insertcnt < PRIME_COUNT) {
      if (isPrime(primesmaxint, primes)) {
        outfile << primesmaxint << '\n';
        primes.push_back(primesmaxint);
        ++insertcnt;
        ++progress;
      }
      ++primesmaxint;
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Operation " << (i + 1) << " completed in " << duration.count() << " seconds" << std::endl;
  }
  // Get peak memory usage (in GB)
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  std::cout << "Peak memory usage: " << (usage.ru_maxrss / 1024.0 / 1024.0) << " GB" << std::endl;
}
