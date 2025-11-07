#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "ml/NaiveBayes.h"
using namespace std;


//pls review this function!!!
//helper function to read the file
void readFile(string filename, vector<string>& labels, vector<string>& emails) {
    //opening the file and preparing variables
    ifstream file(filename);
    string line;
    string label = "";
    string email = "";
    //since this is a large file, we are limiting the amount to be 100k
    int index = 0;

    //reading file
    while (getline(file, line)) {
        //variables to read the possible label (the csv file is fomatted weirdly)
        stringstream ss(line);
        string possibleLabel;
        getline(ss, possibleLabel, ',');

        //if the label is found, it'll push back the previous email
        //and update with the new email values (label and the beginning of
        //the next email)
        if (possibleLabel == "Spam" || possibleLabel == "Ham") {
            if (!label.empty()) {
                labels.push_back(label);
                emails.push_back(email);
                email.clear();
            }
            label = possibleLabel;
            email = line.substr(label.size()+1);
            index++;
        } else {
            //if no label is found, it'll keep reading the file
            email += " " + line;
        }
    }
    //in case the file ends, we want to save the last value
    if (!label.empty()) {
        labels.push_back(label);
        emails.push_back(email);
    }
    file.close();
}

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
    predictionsNaive.predict(userEmail);



    return 0;
}