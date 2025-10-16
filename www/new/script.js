// script.js

document.getElementById('setCookieBtn').addEventListener('click', () => {
    document.cookie = "testCookie=42Webserv; path=/";
    document.getElementById('cookieStatus').textContent = "Cookie 'testCookie' set!";
});

document.getElementById('getCookieBtn').addEventListener('click', () => {
    const cookies = document.cookie.split('; ');
    const cookie = cookies.find(row => row.startsWith('testCookie='));
    const cookieValue = cookie ? cookie.split('=')[1] : null;
    document.getElementById('cookieStatus').textContent = cookieValue ? `Cookie Value: ${cookieValue}` : "Cookie not found.";
});

document.getElementById('sendGet').addEventListener('click', () => {
    fetch('/test', { method: 'GET' })
        .then(response => response.text())
        .then(data => alert(`GET Response: ${data}`))
        .catch(error => console.error('GET Error:', error));
});

document.getElementById('sendPost').addEventListener('click', () => {
    fetch('/test', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ message: 'Hello from POST' })
    })
    .then(response => response.text())
    .then(data => alert(`POST Response: ${data}`))
    .catch(error => console.error('POST Error:', error));
});

document.getElementById('sendPut').addEventListener('click', () => {
    fetch('/test', {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ message: 'Hello from PUT' })
    })
    .then(response => response.text())
    .then(data => alert(`PUT Response: ${data}`))
    .catch(error => console.error('PUT Error:', error));
});

document.getElementById('sendDelete').addEventListener('click', () => {
    fetch('/test', { method: 'DELETE' })
        .then(response => response.text())
        .then(data => alert(`DELETE Response: ${data}`))
        .catch(error => console.error('DELETE Error:', error));
});

document.getElementById('sendCustomHeader').addEventListener('click', () => {
    fetch('/test', {
        method: 'GET',
        headers: { 'X-Custom-Header': '42Test' }
    })
    .then(response => response.text())
    .then(data => alert(`Custom Header Response: ${data}`))
    .catch(error => console.error('Custom Header Error:', error));
});

document.getElementById('uploadFile').addEventListener('click', () => {
    const fileInput = document.getElementById('fileInput');
    const formData = new FormData();
    formData.append('file', fileInput.files[0]);

    fetch('/upload', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => alert(`File Upload Response: ${data}`))
    .catch(error => console.error('File Upload Error:', error));
});

document.getElementById('trigger404').addEventListener('click', () => {
    fetch('/nonexistent')
        .then(response => response.text())
        .then(data => alert(`404 Response: ${data}`))
        .catch(error => console.error('404 Error:', error));
});

document.getElementById('trigger405').addEventListener('click', () => {
    fetch('/test', { method: 'PATCH' })
        .then(response => response.text())
        .then(data => alert(`405 Response: ${data}`))
        .catch(error => console.error('405 Error:', error));
});
