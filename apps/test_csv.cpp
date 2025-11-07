//
// Created by Joshua Betska on 11/6/25.
//
#include <iostream>
#include <unordered_map>
#include "../include/core/CsvReader.h"

int main() {
    CsvReader rdr("./data/raw/spam_Emails_data.csv");
    if (!rdr.readHeader()) {
        std::cerr << "Failed to read CSV header\n";
        return 1;
    }

    const auto& H = rdr.headers();
    std::cout << "Headers (" << H.size() << "):\n";
    for (const auto& h : H) std::cout << " - " << h << '\n';

    std::cout << "\nFirst 5 rows:\n";
    std::unordered_map<std::string,std::string> row;
    for (int i = 0; i < 5 && rdr.next(row); ++i) {
        for (const auto& h : H) {
            auto it = row.find(h);
            std::cout << h << ": " << (it==row.end()? "" : it->second) << " | ";
        }
        std::cout << '\n';
    }
    return 0;
}