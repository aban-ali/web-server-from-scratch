# ⚽ Football Academy Web Server (C + Sockets)

A lightweight multithreaded HTTP server written in C that serves a football academy website and handles form submissions using raw TCP sockets.

This project demonstrates low-level systems programming, networking, and concurrent request handling without relying on external frameworks.

---

## 🚀 Features

* 🔌 Built using **POSIX sockets (TCP)**
* 🧵 **Multithreaded server** using `pthread` for concurrent clients
* 🌐 Handles basic **HTTP/1.1 GET and POST requests**
* 📂 Serves **static files** (HTML, CSS, JS, images)
* 📝 Processes **form submissions (POST)** with custom parsing
* 🎯 Dynamic response generation (token + staff assignment)
* 📦 Minimal dependencies (pure C, standard libraries)

---

## 🧠 Project Overview

This server simulates a **football academy enquiry system**:

* Users visit the homepage (`index.html`)
* Fill out a form (name + phone number)
* Server processes the POST request
* Generates:

  * A **token number**
  * Assigns a **staff member**
  * Returns a dynamic HTML response

---

## 🏗️ Architecture

```
Client (Browser)
        │
        ▼
   TCP Socket (Port 8008)
        │
        ▼
Main Server Loop (accept)
        │
        ▼
Thread per Client (pthread)
        │
        ├── GET  → Serve static files (www/)
        └── POST → Parse JSON → Generate response
```

---

## 📁 Project Structure

```
.
├── server.c
├── www/
│   ├── index.html
│   ├── error.html
│   ├── styles.css
│   └── assets/
└── README.md
```

---

## ⚙️ How It Works

### 🔹 GET Requests

* Parses requested file path
* Serves files from `www/` directory
* Detects MIME types dynamically
* Returns `404` if file not found

### 🔹 POST Requests

* Extracts JSON data from request body:

  ```json
  {
    "name": "John",
    "phone": "1234567890"
  }
  ```
* Generates:

  * Random token
  * Assigns staff from a predefined set of employees (can be connected to a database).
* Returns dynamic HTML response

---

## 🧪 Example Response

```html
Enquiry Details

Name of Customer: John
Phone number: 1234567890
Token Number: 12345
Enquiry Assigned to: Virat Kohli
```

---

## 🛠️ Build & Run

### Compile

```bash
gcc server.c -o server -lpthread
```

### Run

```bash
./server
```

Server will start on:

```
http://localhost:8008
```

---

## 🔍 Key Concepts Demonstrated

* Low-level **network programming**
* **Manual HTTP parsing**
* **Concurrent server design**
* Memory handling in C
* File I/O and streaming
* Basic request routing logic

---

## ⚠️ Limitations

* Not production-ready HTTP parser
* No HTTPS support
* No input validation / security hardening
* Fixed buffer sizes
* Basic MIME type handling

---

## 🔮 Future Improvements

* Add WebSocket support (real-time updates)
* Implement proper HTTP parsing library
* Add logging and monitoring
* Improve security (sanitize inputs, prevent overflow)
* Support REST APIs (JSON responses)
* Containerization (Docker)

---
