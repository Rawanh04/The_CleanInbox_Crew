#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "core/CsvReader.h"
#include "ml/NaiveBayes.h"
#include "ml/Rocchio.h"
#include "core/httplib.h"
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

    Rocchio roc;
    roc.fit(emails, labels);

    // create http server
    httplib::Server svr;

    //serve static frontend files
    svr.set_mount_point("/", "./frontend");

    // scan and classify pasted/uploaded text
    svr.Post("/scan", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            //extract json file from request
            auto body = req.body;
            size_t start = body.find("\"text\":\"");
            if (start == string::npos) {
                res.status = 400;
                res.set_content("{\"error\": Missing text field\"}", "application/json");
                return;
            }

            start += 8;
            size_t end = body.find("\"", start);
            string emailText = body.substr(start, end - start);

            //run both models
            string naiveBayes_result = predictionsNaive.predict(emailText);
            string roc_result = roc.predict(emailText);

            //create json response
            string json = "{";
            json += "\"NaiveBayes\": \"" + naiveBayes_result + "\", ";
            json += "\"Rocchio\": \"" + roc_result + "\", ";
            json += "\"explanation\": \"Naive Bayes uses word frequency while Rocchio uses vector similarity.\"";
            json += "}";

            res.set_content(json, "application/json");
        } catch (...) {
            res.status = 500;
            res.set_content("{\"error\": Unknown Error}", "application/json");
        }
    });

    return 0;
}


