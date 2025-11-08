// this script handles interaction !! wowza !!

//wait until the page has fully loaded
document.addEventListener("DOMContentLoaded", () => {

    // grab elements from DOM if they exist
    const scanBtn = document.getElementById("scanBtn");
    const loadTestBtn = document.getElementById("loadTestBtn");
    const exportBtn = document.getElementById("exportBtn");

    // handles scan button
    // user pastes email text and scans it
    if (scanBtn) {
        scanBtn.addEventListener("click", async () => {
            const text = document.getElementById("emailText").value.trim();

            //if empty, show alert
            if (!text) {
                alert("Please paste or upload an email in .txt format first!")
                return;
            }

            try {
                // sends post request to backend
                const response = await fetch("/scan", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ text }), //sends text as JSON
                });

                // parse backend response
                const data = await response.json();

                //save results to show on results page
                localStorage.setItem("scanResults", JSON.stringify(data));

                // redirects user to the results page
                window.location.href = "result.html";
            } catch (error) {
                console.error("Scan request failed:" ,error);
                alert("Error!! Couldn't connect to backend!!")
            }
        })
    }

    // handle load testing data button & loads example json
    if (loadTestBtn) {
        loadTestBtn.addEventListener("click", async () => {
            try {
                //request data from backend
                const response = await fetch("/test");
                const data = await response.json();

                // save test results and go to results page
                localStorage.setItem("scanResults", JSON.stringify(data));
                window.location.href = "results.html";
            } catch (error) {
                console.error("Test data request failed:" , error);
                alert("Error!! Couldn't load test data!!");
            }
        })
    }

    // populate results.html with backend response
    if (window.location.pathname.endsWith("results.html")) {
        // gets the results
        const results = JSON.parse(localStorage.getItem("scanResults") || "{}");

        //fill in result values
        document.getElementById("naiveBayesResult").textContent = results.nb || "N/A";
        document.getElementById("rocchioResult").textContent = results.rocchio || "N/A";
        document.getElementById("explanation").textContent = results.explanation || "No explanation provided.";

        // enable "export results" button
        if (exportBtn) {
            exportBtn.addEventListener("click", () => {
                const blob = new Blob([JSON.stringify(results, null, 2)], {
                    type: "application/json",
                });

                const link = document.createElement("a");
                link.href = URL.createObjectURL(blob);
                link.download = "results.json";
                link.click();
            })
        }
    }

    // populate email.html to show email details
    if (window.location.pathname.endsWith("email.html")) {
        const emailData = JSON.parse(localStorage.getItem("emailResults") || "{}");

        //update email detail fields
        document.getElementById("emailStatus").textContent = emailData.status || "Unknown";
        document.getElementById("emailBody").textContent = emailData.text || "";
        document.getElementById("emailExplanation").textContent = emailData.explanation || "";
    }
})

