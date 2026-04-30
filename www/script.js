const formDiv = document.getElementById("callbackForm");
const responseDiv = document.getElementById("response");


// Form submission
document.getElementById("callbackForm").addEventListener("submit", async (e) => {
    e.preventDefault();

    responseDiv.style.display = "none";

    const data = {
        name: document.getElementById("name").value,
        phone: document.getElementById("phone").value
    };
    

    try {
        const res = await fetch("/api/request-call", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(data)
        });
        const html = await res.text();
        console.log(html);
        responseDiv.innerHTML = html;
        responseDiv.style.display = "block";
        
        document.getElementById("name").value=null;
        document.getElementById("phone").value=null;
    } catch (err){
        console.log("Error: ", err.message);
        window.location.href = "error.html";
    }
});

// Chat toggle
const chatIcon = document.getElementById("chatIcon");
const chatbot = document.getElementById("chatbot");
const closeChat = document.getElementById("closeChat");

chatIcon.onclick = () => {
    chatbot.style.display = "block";
};

closeChat.onclick = () => {
    chatbot.style.display = "none";
};

// Chatbot messaging
const chatInput = document.getElementById("chatInput");
const chatBody = document.getElementById("chatBody");

chatInput.addEventListener("keypress", async (e) => {
    if (e.key === "Enter") {
        const message = chatInput.value;

        chatBody.innerHTML += `<div>You: ${message}</div>`;

        const responses = [
            "Don't have enough budget for LLM services. Please Donate to support the project.",
            "Mahh man!! I wish I could help you, but give me some money first. (Minimum just $999/token)",
            "How can I assist you? Oh wait, I can't until you donate. Just kidding, but seriously, donate to keep me running!",
            "Do not text again until you donate. I can only respond to donors."
        ];

        chatBody.innerHTML += `<div>Bot: ${responses[Math.floor(Math.random() * responses.length)]}</div>`;

        chatInput.value = "";
        chatBody.scrollTop = chatBody.scrollHeight;
    }
});