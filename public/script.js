async function submitLogin(endpoint, username, password, resultBox) {
    resultBox.className = "result";
    resultBox.textContent = "Checking...";

    const body = new URLSearchParams();
    body.append("username", username);
    body.append("password", password);

    try {
        const response = await fetch(endpoint, {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: body.toString()
        });

        const data = await response.json();

        if (response.ok) {
            resultBox.className = "result result-success";
            resultBox.textContent = "HTTP " + response.status + ": " + data.message;
        } else {
            resultBox.className = "result result-rejected";
            resultBox.textContent = "HTTP " + response.status + ": " + data.message;
        }
    } catch (error) {
        resultBox.className = "result result-rejected";
        resultBox.textContent = "Error: Could not reach the server.";
    }
}

document.getElementById("vulnerable-form").addEventListener("submit", function (e) {
    e.preventDefault();
    submitLogin(
        "/vulnerable/login",
        document.getElementById("vuln-username").value,
        document.getElementById("vuln-password").value,
        document.getElementById("vuln-result")
    );
});

document.getElementById("secure-form").addEventListener("submit", function (e) {
    e.preventDefault();
    submitLogin(
        "/secure/login",
        document.getElementById("sec-username").value,
        document.getElementById("sec-password").value,
        document.getElementById("sec-result")
    );
});

document.querySelectorAll(".demo-bar button[data-user]").forEach(function (btn) {
    btn.addEventListener("click", function () {
        const u = btn.getAttribute("data-user");
        const p = btn.getAttribute("data-pass");
        document.getElementById("vuln-username").value = u;
        document.getElementById("vuln-password").value = p;
        document.getElementById("sec-username").value = u;
        document.getElementById("sec-password").value = p;
    });
});

