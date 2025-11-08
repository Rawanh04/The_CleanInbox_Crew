//
// Rocchio.h
//

#ifndef ROCCHIO_H
#define ROCCHIO_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class Rocchio {
public:
    Rocchio() = default;

    // Train on parallel vectors: texts[i] has label labels[i]
    void fit(const vector<string>& texts, const vector<string>& labels);

    // Predict label for a single email
    string predict(const string& text) const;

private:
    // label -> (term -> weight)
    unordered_map<string, unordered_map<string, double>> centroids_;
    // label -> ||centroid||
    unordered_map<string, double> centroidNorm_;
    // term -> idf weight
    unordered_map<string, double> idf_;

    unordered_set<string> stopwords_ {
        "the","a","an","and","or","to","of","in","is","it",
        "for","on","at","this","that","with","be","as","are",
        "i","you","he","she","they","we","from","by"
    };

    static vector<string> tokenize(const string& text);
    bool isStopword(const string& w) const {
        return stopwords_.find(w) != stopwords_.end();
    }

    static double dot(const unordered_map<string,double>& a,
                      const unordered_map<string,double>& b);
    static double norm(const unordered_map<string,double>& a);
};

#endif // ROCCHIO_H

