//
// Created by Joshua Betska on 11/6/25.
//  a simple csv reader with header +quoted fields

#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

using namespace std;

class CsvReader {
public:
  explicit CsvReader(const string& path);

  bool ok() const { return in_.is_open(); }
  bool readHeader();                         // call once
  bool next(unordered_map<string,string>&);  // next full record

  const vector<string>& headers() const { return headers_; }

private:
  ifstream in_;
  vector<string> headers_;
  string chunk_; // accumulate lines until quotes are balanced

  static bool quotesBalanced(const string& s);
  static vector<string> parseCsvLine(const string& line);
  static string trim(const string& s);
};

void readFile(string filename, vector<string>& labels, vector<string>& emails);


#endif // CSVREADER_H