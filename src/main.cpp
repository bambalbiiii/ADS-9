// Copyright 2022 NNTU-CS
#include <unistd.h>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "tree.h"

int main() {
  std::vector<char> init = {'1', '2', '3'};
  PMTree example(init);

  auto variants = getAllPerms(example);
  for (auto& seq : variants) {
    for (char c : seq) std::cout << c;
    std::cout << "  ";
  }
  std::cout << "\n\n";

  std::cout << "getPerm1(1): ";
  for (char c : getPerm1(example, 1)) std::cout << c;
  std::cout << "\n";

  std::cout << "getPerm2(2): ";
  for (char c : getPerm2(example, 2)) std::cout << c;
  std::cout << "\n\n";

  std::cout << "n;getAllPerms(s);getPerm1(s);getPerm2(s)" << std::endl;

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == nullptr) {
    std::cerr << "Ошибка getcwd\n";
    return 1;
  }
  std::string base(cwd);
  std::string csv_path = base + "/result/data.csv";
  std::string png_path = base + "/result/plot.png";
  std::string gp_path  = base + "/result/plot.gp";

  std::ofstream csv(csv_path);
  csv << "n;getAllPerms;getPerm1;getPerm2\n";

  for (int n = 3; n <= 10; ++n) {
    std::vector<char> letters;
    for (int i = 0; i < n; ++i) letters.push_back('a' + i);
    PMTree tree(letters);

    std::mt19937 gen(42);
    std::uniform_int_distribution<int64_t> dist(1,
      static_cast<int64_t>(fact(n)));
    int target = static_cast<int>(dist(gen));

    auto t0 = std::chrono::high_resolution_clock::now();
    getAllPerms(tree);
    auto t1 = std::chrono::high_resolution_clock::now();

    getPerm1(tree, target);
    auto t2 = std::chrono::high_resolution_clock::now();

    getPerm2(tree, target);
    auto t3 = std::chrono::high_resolution_clock::now();

    double d1 = std::chrono::duration<double>(t1 - t0).count();
    double d2 = std::chrono::duration<double>(t2 - t1).count();
    double d3 = std::chrono::duration<double>(t3 - t2).count();

    std::cout << n << ";"
              << std::fixed << std::setprecision(6)
              << d1 << ";" << d2 << ";" << d3 << "\n";

    csv << n << ";"
        << std::fixed << std::setprecision(9)
        << d1 << ";" << d2 << ";" << d3 << "\n";
  }
  csv.close();

  std::ofstream gp(gp_path);
  gp << "set terminal png size 900,600\n";
  gp << "set output '" << png_path << "'\n";
  gp << "set title 'Time vs alphabet size'\n";
  gp << "set xlabel 'n'\n";
  gp << "set ylabel 'Time (s)'\n";
  gp << "set logscale y\n";
  gp << "set grid\n";
  gp << "set datafile separator ';'\n";
  gp << "set key top left\n";
  gp << "plot '" << csv_path
     << "' every ::1 using 1:2 with linespoints lw 2 pt 7"
     << " title 'getAllPerms',\\\n";
  gp << "     '" << csv_path
     << "' every ::1 using 1:3 with linespoints lw 2 pt 5"
     << " title 'getPerm1',\\\n";
  gp << "     '" << csv_path
     << "' every ::1 using 1:4 with linespoints lw 2 pt 9"
     << " title 'getPerm2'\n";
  gp.close();

  std::string cmd = "gnuplot " + gp_path;
  int ret = system(cmd.c_str());
  if (ret == 0) {
    std::cout << "\nPlot saved to result/plot.png\n";
  } else {
    std::cerr << "\ngnuplot error (code " << ret << ")\n";
  }

  return 0;
}
