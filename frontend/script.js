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
                window.location.href = "results.html";
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
        if (window.location.pathname.endsWith("results.html")) {
            const results = JSON.parse(localStorage.getItem("scanResults") || "{}");
            const container = document.querySelector(".results-container");
            container.innerHTML = "";

            // Case 1: /test endpoint (multiple samples)
            if (results.samples && Array.isArray(results.samples) && results.samples.length > 0) {
                results.samples.forEach((sample, i) => {
                    const div = document.createElement("div");
                    div.classList.add("result-box");
                    div.innerHTML = `
                <h3>Sample ${i + 1}</h3>
                <p><strong>True label:</strong> ${sample.label}</p>
                <p><strong>Naive Bayes:</strong> ${sample.nb}</p>
                <p><strong>Rocchio:</strong> ${sample.rocchio}</p>
                <p>${sample.explanation}</p>
            `;
                    container.appendChild(div);
                });
            }

            // Case 2: /scan endpoint (single prediction)
            else if (results.nb || results.rocchio) {
                const div = document.createElement("div");
                div.classList.add("result-box");
                div.innerHTML = `
            <h3>Scan Results</h3>
            <p><strong>Naive Bayes:</strong> ${results.nb || "N/A"}</p>
            <p><strong>Rocchio:</strong> ${results.rocchio || "N/A"}</p>
            <p>${results.explanation || "No explanation provided."}</p>
        `;
                container.appendChild(div);
            }

            // Fallback case
            else {
                container.innerHTML = "<p>No results found!</p>";
            }
        }


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

    // file upload handler
    const fileInput = document.getElementById("fileInput");
    const dropZone = document.getElementById("dropZone");
    const emailText = document.getElementById("emailText");
    const fileLabel = document.getElementById("fileLabel");

    if (fileInput && emailText) {

        const handleFile = async (file) => {
            if (!file || !file.name.endsWith(".txt")) {
                alert("Please upload a valid .txt file!");
                return;
            }

            const reader = new FileReader();
            reader.onload = async (e) => {
                const text = e.target.result.trim();
                if (!text) {
                    alert("File is empty!");
                    return;
                }

                emailText.value = text; // fill textarea

                try {
                    // send POST request to /scan
                    const response = await fetch("/scan", {
                        method: "POST",
                        headers: { "Content-Type": "application/json" },
                        body: JSON.stringify({ text }),
                    });

                    const data = await response.json();
                    localStorage.setItem("scanResults", JSON.stringify(data));
                    window.location.href = "results.html";

                } catch (err) {
                    console.error("Scan request failed:", err);
                    alert("Error connecting to backend!");
                }
            };
            reader.readAsText(file);
        };

        // handle input file selection
        fileInput.addEventListener("change", e => {
            handleFile(e.target.files[0]);
        });

        // handle drag & drop
        if (dropZone && fileLabel) {
            ["dragenter", "dragover"].forEach(eventName => {
                dropZone.addEventListener(eventName, e => {
                    e.preventDefault();
                    dropZone.classList.add("dragover");
                    fileLabel.textContent = "📂 Drop your .txt file here!";
                });
            });

            ["dragleave", "drop"].forEach(eventName => {
                dropZone.addEventListener(eventName, e => {
                    e.preventDefault();
                    dropZone.classList.remove("dragover");
                    fileLabel.textContent = "📂 Open file / Drop file here";
                });
            });

            dropZone.addEventListener("drop", e => {
                const file = e.dataTransfer.files[0];
                handleFile(file);
            });
        }
    }


})

