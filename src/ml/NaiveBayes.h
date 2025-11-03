//
// Created by rawan ali on 10/27/25.
//

#ifndef NAIVEBAYES_H
#define NAIVEBAYES_H
#include <set>
#include <vector>
#include <unordered_map>

using namespace std;

class NaivesBayes {
    unordered_map<string, int> totalWords;
    unordered_map<string, unordered_map<string, int>> wordCount;
    unordered_map<string, double> priors;
    set<string> words;
    set<string> categories;
    set<string> fillerWords = {"and", "but", "or", "so",
        "because", "for", "it", "the", "is", "to",
    "of", "in", "a", ":", ",", "."};


public:
    void fit(const vector<string>& x_train, const vector<string>& y_train);
    string predict(const string& email);
};

#endif //NAIVEBAYES_H
