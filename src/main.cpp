#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "core/CsvReader.h"
#include "ml/NaiveBayes.h"
#include "ml/Rocchio.h"
#include "httplib.h"
using namespace std;

int main() {
    //vectors for dataset
    vector<string> labels;
    vector<string> emails;

    //reading files
    string filename = "data/raw/spam_Emails_data.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cout <<"help" << endl;
    }
    readFile(filename,labels,emails);

    //email that will be analyzed
    string userEmail;

    NaivesBayes predictionsNaive;
    predictionsNaive.fit(emails,labels);

    //for the naive bayes algorithm, pls use the predict function:
    //it will return a string that either says Ham or Spam
    predictionsNaive.predict(userEmail);



    return 0;
}


