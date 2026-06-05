
// Copyright 2022 NNTU-CS
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include "tree.h"
 
int main() {
    // --- Демонстрация ---
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
 
    // --- Эксперимент ---
    std::cout << "n;getAllPerms(s);getPerm1(s);getPerm2(s)" << std::endl;
 
    // Получаем абсолютный путь заранее для CSV
    char cwd_early[1024];
    if (getcwd(cwd_early, sizeof(cwd_early)) == nullptr) {
        std::cerr << "Ошибка getcwd\n"; return 1;
    }
    std::string csv_early = std::string(cwd_early) + "/result/data.csv";
 
    // Записываем данные в CSV для gnuplot
    std::ofstream csv(csv_early);
    csv << "n;getAllPerms;getPerm1;getPerm2\n";
 
    for (int n = 3; n <= 10; ++n) {
        std::vector<char> letters;
        for (int i = 0; i < n; ++i) letters.push_back('a' + i);
        PMTree tree(letters);
 
        std::mt19937 gen(42);
        std::uniform_int_distribution<long long> dist(1,
            static_cast<long long>(fact(n)));
        int target = static_cast<int>(dist(gen));
 
        // Замер getAllPerms
        auto t0 = std::chrono::high_resolution_clock::now();
        getAllPerms(tree);
        auto t1 = std::chrono::high_resolution_clock::now();
 
        // Замер getPerm1
        getPerm1(tree, target);
        auto t2 = std::chrono::high_resolution_clock::now();
 
        // Замер getPerm2
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
 
    // --- Построение графика через gnuplot ---
    // Получаем абсолютный путь к текущей директории
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        std::cerr << "Ошибка получения текущей директории\n";
        return 1;
    }
    std::string base(cwd);
    std::string csv_path  = base + "/result/data.csv";
    std::string png_path  = base + "/result/plot.png";
    std::string gp_path   = base + "/result/plot.gp";
 
    std::ofstream gp(gp_path);
    gp << "set terminal png size 900,600\n";
    gp << "set output '" << png_path << "'\n";
    gp << "set title 'Время работы функций от размера алфавита'\n";
    gp << "set xlabel 'n (размер алфавита)'\n";
    gp << "set ylabel 'Время (с)'\n";
    gp << "set logscale y\n";
    gp << "set grid\n";
    gp << "set datafile separator ';'\n";
    gp << "set key top left\n";
    gp << "plot '" << csv_path << "' every ::1 using 1:2 with linespoints "
          "lw 2 pt 7 title 'getAllPerms',\\\n";
    gp << "     '" << csv_path << "' every ::1 using 1:3 with linespoints "
          "lw 2 pt 5 title 'getPerm1',\\\n";
    gp << "     '" << csv_path << "' every ::1 using 1:4 with linespoints "
          "lw 2 pt 9 title 'getPerm2'\n";
    gp.close();
 
    std::string cmd = "gnuplot " + gp_path;
    int ret = system(cmd.c_str());
    if (ret == 0) {
        std::cout << "\nГрафик сохранён в result/plot.png\n";
    } else {
        std::cerr << "\nОшибка запуска gnuplot (код " << ret << ")\n";
    }
 
    return 0;
}
