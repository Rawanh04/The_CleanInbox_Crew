//
// Created by Joshua Betska on 11/7/25.

//
// Rocchio.cpp
//

#include "../../include/ml/Rocchio.h"
#include <cctype>
#include <cmath>

namespace {
    string lowerStr(const string& s) {
        string out;
        out.reserve(s.size());
        for (char c : s) {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        return out;
    }
}

// basic tokenizer: alnum runs, lowercased
vector<string> Rocchio::tokenize(const string& text) {
    vector<string> tokens;
    string cur;
    string s = lowerStr(text);

    for (char c : s) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            cur.push_back(c);
        } else if (!cur.empty()) {
            tokens.push_back(cur);
            cur.clear();
        }
    }
    if (!cur.empty()) tokens.push_back(cur);

    return tokens;
}

double Rocchio::dot(const unordered_map<string,double>& a,
                    const unordered_map<string,double>& b) {
    const auto* p = &a;
    const auto* q = &b;
    if (b.size() < a.size()) {
        p = &b;
        q = &a;
    }
    double s = 0.0;
    for (const auto& kv : *p) {
        auto it = q->find(kv.first);
        if (it != q->end()) {
            s += kv.second * it->second;
        }
    }
    return s;
}

double Rocchio::norm(const unordered_map<string,double>& a) {
    double ss = 0.0;
    for (const auto& kv : a) {
        ss += kv.second * kv.second;
    }
    return ss > 0.0 ? std::sqrt(ss) : 0.0;
}

void Rocchio::fit(const vector<string>& texts, const vector<string>& labels) {
    centroids_.clear();
    centroidNorm_.clear();
    idf_.clear();

    const size_t n = texts.size();
    if (n == 0 || n != labels.size()) return;

    // per-doc tf and set of unique terms for df counting
    vector<unordered_map<string,double>> docTf(n);
    vector<string> docLabel(n);
    unordered_map<string,int> df;
    unordered_map<string,int> classCount;

    for (size_t i = 0; i < n; ++i) {
        docLabel[i] = labels[i];
        classCount[labels[i]]++;

        auto tokens = tokenize(texts[i]);
        if (tokens.empty()) continue;

        auto& tf = docTf[i];
        unordered_set<string> seen;

        for (const auto& w : tokens) {
            if (isStopword(w)) continue;
            tf[w] += 1.0;
            seen.insert(w);
        }

        for (const auto& w : seen) {
            df[w] += 1;
        }
    }

    // compute idf
    const double N = static_cast<double>(n);
    for (const auto& kv : df) {
        // smooth to avoid div-by-zero and overweighting rare noise
        idf_[kv.first] = std::log((N + 1.0) / (kv.second + 1.0)) + 1.0;
    }

    // accumulate class centroids (TF-IDF)
    for (size_t i = 0; i < n; ++i) {
        auto& tf = docTf[i];
        if (tf.empty()) continue;

        // L1 normalize TF
        double sum = 0.0;
        for (const auto& kv : tf) sum += kv.second;
        if (sum > 0.0) {
            for (auto& kv : tf) kv.second /= sum;
        }

        const string& y = docLabel[i];
        auto& centroid = centroids_[y];

        for (const auto& kv : tf) {
            auto idfIt = idf_.find(kv.first);
            if (idfIt == idf_.end()) continue;
            double w = kv.second * idfIt->second;
            centroid[kv.first] += w;
        }
    }

    // average and precompute norms
    for (auto& kv : centroids_) {
        const string& y = kv.first;
        auto& c = kv.second;
        int cnt = classCount[y];
        if (cnt <= 0) continue;

        double ss = 0.0;
        for (auto& t : c) {
            t.second /= static_cast<double>(cnt);
            ss += t.second * t.second;
        }
        centroidNorm_[y] = ss > 0.0 ? std::sqrt(ss) : 0.0;
    }
}

string Rocchio::predict(const string& text) const {
    if (centroids_.empty() || idf_.empty()) {
        return "unknown";
    }

    auto tokens = tokenize(text);
    if (tokens.empty()) {
        return "unknown";
    }

    unordered_map<string,double> tf;

    for (const auto& w : tokens) {
        if (isStopword(w)) continue;
        tf[w] += 1.0;
    }

    if (tf.empty()) return "unknown";

    double sum = 0.0;
    for (const auto& kv : tf) sum += kv.second;
    if (sum > 0.0) {
        for (auto& kv : tf) kv.second /= sum;
    }

    // build TF-IDF vector
    unordered_map<string,double> v;
    v.reserve(tf.size());
    for (const auto& kv : tf) {
        auto idfIt = idf_.find(kv.first);
        if (idfIt == idf_.end()) continue;
        v[kv.first] = kv.second * idfIt->second;
    }
    if (v.empty()) return "unknown";

    double vNorm = norm(v);
    if (vNorm == 0.0) return "unknown";

    string bestLabel;
    double bestScore = -1.0;

    for (const auto& ck : centroids_) {
        const string& y = ck.first;
        const auto& c = ck.second;

        auto itNorm = centroidNorm_.find(y);
        if (itNorm == centroidNorm_.end() || itNorm->second == 0.0) continue;

        double score = dot(v, c) / (vNorm * itNorm->second);
        if (score > bestScore) {
            bestScore = score;
            bestLabel = y;
        }
    }

    return bestLabel.empty() ? "unknown" : bestLabel;
}

