//
// Created by rawan ali on 10/27/25.
//
#include "ml/NaiveBayes.h"
#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

//taking the email and making each word an entry in a vector
void tokenize(const string& s, vector<string>& tokens, set<string>& fillerWords) {
    istringstream iss(s);
    string token;
    //iterating through the string
    while (getline(iss, token, ' ')) {
        //making sure lowercased and uppercased words are the same
        for (char& c : token) {
            c = tolower(c);
        }
        //removing punctuation
        size_t start = 0, end = token.size() - 1;
        while (start < token.size() && !isalpha(token[start])) {start++;}
        while (end > start && !isalpha(token[end])) {end--;}
        if (start > end) {continue;}
        token = token.substr(start, end - start + 1);

        //removing punctuation on words
        token.erase(remove_if(token.begin(), token.end(),
            [](char c){return !isalpha(c);}),token.end());

        //making sure the token is not empty and not a filler word
        if (fillerWords.find(token) == fillerWords.end() && token.size() > 0) {
            tokens.push_back(token);
        }
    }
}

//algorithm that takes in the data and analyzes it
void NaivesBayes::fit(const vector<string>& x_train, const vector<string>& y_train) {
    int n_sample = x_train.size();

    //adding the 2 categories, ham and spam
    categories.insert("ham");
    categories.insert("spam");
    totalWords["ham"] = 0;
    totalWords["spam"] = 0;

    //for loop that adds the words of the emails into a data structure
    //here it would be two unordered_map
    for (int i = 0; i < n_sample; i++) {
        string label = y_train[i];
        for (char& c : label) {
            c = tolower(c);
        }
        vector<string> words;
        tokenize(x_train[i], words, fillerWords);

        for (const auto& word : words) {
            wordCount[label][word]++;
            totalWords[label]++;
            this->words.insert(word);
        }
    }

    //going over the word count and calculating the prior for each label
    for (const auto& label : categories) {
        double count = 0.0;
        for (const auto& y : y_train) {
            string l = y;
            for (char& c : l) {
                c = tolower(c);
            }
            if (l == label) {
                count++;
            }
        }
        priors[label] = count/n_sample;
    }
}

//predicting if the email is ham or spam
string NaivesBayes::predict(const string& email) {
    vector<string> words;
    tokenize(email, words, fillerWords);

    //using the priors and sums to calculate the score of each label based on the formula
    //https://en.wikipedia.org/wiki/Naive_Bayes_classifier
    double scoreSpam = log(priors["spam"]);
    double scoreHam = log(priors["ham"]);

    for (string word : words) {
        scoreSpam += log((wordCount["spam"][word]+1.0)/(totalWords["spam"]+this->words.size()));
        scoreHam += log((wordCount["ham"][word]+1.0)/(totalWords["ham"]+this->words.size()));
    }

    //returning if ham or spam
    if (scoreSpam > scoreHam) {
        return "Spam";
    }
    return "Ham";
}