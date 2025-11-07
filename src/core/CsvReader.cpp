//
// Created by Joshua Betska on 11/6/25.
//
#include "../../include/core/CsvReader.h"
#include <cctype>

static inline bool isSp(unsigned char c){ return std::isspace(c)!=0; }

CsvReader::CsvReader(const string& path)
    : in_(path, ios::binary) {}

string CsvReader::trim(const string& s){
    size_t i=0, j=s.size();
    while(i<j && isSp((unsigned char)s[i])) ++i;
    while(j>i && isSp((unsigned char)s[j-1])) --j;
    return s.substr(i, j-i);
}

bool CsvReader::quotesBalanced(const string& s){
    size_t c=0;
    for(size_t i=0;i<s.size();++i){
        if(s[i]=='"'){
            if(i+1<s.size() && s[i+1]=='"'){ ++i; continue; }
            ++c;
        }
    }
    return (c%2)==0;
}

vector<string> CsvReader::parseCsvLine(const string& line){
    vector<string> out; out.reserve(8);
    string cur; bool inq=false;
    for(size_t i=0;i<line.size();++i){
        char ch=line[i];
        if(inq){
            if(ch=='"'){
                if(i+1<line.size() && line[i+1]=='"'){ cur.push_back('"'); ++i; }
                else inq=false;
            } else cur.push_back(ch);
        } else {
            if(ch=='"') inq=true;
            else if(ch==','){ out.push_back(cur); cur.clear(); }
            else cur.push_back(ch);
        }
    }
    out.push_back(cur);
    return out;
}

bool CsvReader::readHeader(){
    if(!in_) return false;
    string h; if(!getline(in_, h)) return false;
    headers_ = parseCsvLine(trim(h));
    chunk_.clear();
    return !headers_.empty();
}

bool CsvReader::next(unordered_map<string,string>& row){
    row.clear();
    if(!in_) return false;

    string line;
    while(true){
        if(!getline(in_, line)){
            if(chunk_.empty()) return false;
            if(!quotesBalanced(chunk_)) return false;
            auto cols = parseCsvLine(chunk_);
            for(size_t i=0;i<headers_.size();++i)
                row[headers_[i]] = (i<cols.size()? cols[i] : "");
            chunk_.clear();
            return true;
        }
        if(!chunk_.empty()) chunk_.push_back('\n');
        chunk_ += line;

        if(quotesBalanced(chunk_)){
            auto cols = parseCsvLine(chunk_);
            for(size_t i=0;i<headers_.size();++i)
                row[headers_[i]] = (i<cols.size()? cols[i] : "");
            chunk_.clear();
            return true;
        }
    }
}