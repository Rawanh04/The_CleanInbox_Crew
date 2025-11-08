//
// Created by Joshua Betska on 11/7/25.
//

//
// test_rocchio.cpp
//

#include <iostream>
#include <unordered_map>
#include "../include/core/CsvReader.h"
#include "../include/ml/Rocchio.h"

int main() {
    CsvReader rdr("../data/raw/spam_Emails_data.csv");

    if (!rdr.readHeader()) {
        std::cerr << "test_rocchio: failed to read CSV header\n";
        return 1;
    }

    const auto& headers = rdr.headers();
    std::string textCol = "text";
    std::string labelCol = "label";

    std::vector<std::string> texts;
    std::vector<std::string> labels;

    std::unordered_map<std::string,std::string> row;
    int limit = 2000; // enough to get a feel without loading everything

    while (limit-- > 0 && rdr.next(row)) {
        auto itText = row.find(textCol);
        auto itLabel = row.find(labelCol);
        if (itText == row.end() || itLabel == row.end()) continue;
        texts.push_back(itText->second);
        labels.push_back(itLabel->second);
    }

    if (texts.empty()) {
        std::cerr << "test_rocchio: no rows loaded; check path/headers.\n";
        return 1;
    }

    Rocchio roc;
    roc.fit(texts, labels);

    std::cout << "Loaded " << texts.size()
              << " samples for Rocchio test.\n";

    std::vector<std::string> samples = {
        "Congratulations! You have been selected to win a free iPhone. Click here now.",
        "Hey, just checking if we are still on for the meeting tomorrow.",
        "URGENT: Your account has been compromised. Verify your password immediately.",
        "Can you send me the notes from today's lecture?"
    };

    for (const auto& s : samples) {
        std::string y = roc.predict(s);
        std::cout << "[" << y << "] " << s << "\n";
    }

    return 0;
}
