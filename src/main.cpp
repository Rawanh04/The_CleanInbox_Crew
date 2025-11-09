/// --- Windows & httplib compatibility macros ---
#define NOMINMAX
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0
#define CPPHTTPLIB_NO_CREATEFILE2

// Include Winsock2 BEFORE windows.h
#include <winsock2.h>
#include <ws2tcpip.h>
// --- end of Windows prep ---


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

#ifdef _WIN32
    WSADATA wsaData;
    int wsaErr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaErr != 0) {
        std::cerr << "WSAStartup failed: " << wsaErr << std::endl;
        return 1;
    }
#endif

    //vectors for dataset
    vector<string> labels;
    vector<string> emails;

    //reading files
    string filename = "C:/Users/17546/CLionProjects/The_CleanInbox_Crew/data/raw/spam_Emails_data.csv"; //replace with the exact path location
    ifstream file(filename);
    if (!file.is_open()) {
        cout <<"help" << endl;
    }
    cout << "reading csv" << endl;
    readFile(filename,labels,emails);

    //email that will be analyzed
    string userEmail;

    cout << "training nb" << endl;
    NaivesBayes predictionsNaive;
    predictionsNaive.fit(emails,labels);

    //for the naive bayes algorithm, pls use the predict function:
    //it will return a string that either says Ham or Spam
    predictionsNaive.predict(userEmail);

    cout << "training rocchio" << endl;
    Rocchio roc;
    roc.fit(emails, labels);

    // create http server
    httplib::Server svr;

    cout << "starting server" << endl;

    //serve static frontend files
    svr.set_mount_point("/", "../frontend");

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
            json += "\"nb\": \"" + naiveBayes_result + "\", ";
            json += "\"rocchio\": \"" + roc_result + "\", ";
            json += "\"explanation\": \"Naive Bayes uses word frequency while Rocchio uses vector similarity.\"";
            json += "}";

            res.set_content(json, "application/json");
        } catch (...) {
            res.status = 500;
            res.set_content("{\"error\": Unknown Error}", "application/json");
        }
    });

    svr.Get("/test", [&](const httplib::Request&, httplib::Response& res) {
    try {
        if (emails.empty() || labels.empty()) {
            res.status = 500;
            res.set_content("{\"error\": \"Dataset not loaded!\"}", "application/json");
            return;
        }

        // create array of test samples
        std::ostringstream json;
        json << "{ \"samples\": [";

        // test up to 5 examples from dataset
        int numSamples = std::min(5, (int)emails.size());
        srand((unsigned)time(nullptr));

        for (int i = 0; i < numSamples; ++i) {
            int idx = rand() % emails.size();

            std::string emailText = emails[idx];
            std::string trueLabel = labels[idx];

            std::string nb_pred = predictionsNaive.predict(emailText);
            std::string roc_pred = roc.predict(emailText);

            std::ostringstream explanation;
            explanation << "True label: " << trueLabel
                        << ". Naive Bayes says: " << nb_pred
                        << ". Rocchio says: " << roc_pred << ".";

            json << "{"
                 << "\"nb\": \"" << nb_pred << "\", "
                 << "\"rocchio\": \"" << roc_pred << "\", "
                 << "\"label\": \"" << trueLabel << "\", "
                 << "\"explanation\": \"" << explanation.str() << "\""
                 << "}";

            if (i < numSamples - 1) json << ",";
        }

        json << "] }";
        res.set_content(json.str(), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "Error in /test: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("{\"error\": \"Exception during test endpoint.\"}", "application/json");
    }
});




    cout << "server running at http://localhost:5000/" << endl;
    svr.listen("0.0.0.0", 5000);

#ifdef _WIN32
    WSACleanup();
#endif


    return 0;
}


