// Helper: display text in shared response box
function showResponse(text) {
  const box = document.getElementById('sharedResponse');
  box.textContent = text;
}

// GET request
function doGet() {
  const path = document.getElementById('getPath').value;
  if (!path) return showResponse('Please enter a GET path.');
  
  fetch(path)
    .then(res => res.text())
    .then(text => showResponse(text))
    .catch(err => showResponse('Error: ' + err));
}

// POST file upload
function doPost() {
  const formData = new FormData();
  const img = document.getElementById('imageFile').files[0];
  const vid = document.getElementById('videoFile').files[0];
  if (img) formData.append('image', img);
  if (vid) formData.append('video', vid);

  fetch('/', { method: 'POST', body: formData })
    .then(res => res.text())
    .then(text => showResponse(text))
    .catch(err => showResponse('Error: ' + err));
}

// Multipart POST
function doMultipart() {
  const key = document.getElementById('mpKey').value;
  const value = document.getElementById('mpValue').value;
  if (!key || !value) return showResponse('Please fill both key and value.');
  
  const formData = new FormData();
  formData.append(key, value);

  fetch('/', { method: 'POST', body: formData })
    .then(res => res.text())
    .then(text => showResponse(text))
    .catch(err => showResponse('Error: ' + err));
}

// DELETE request
function doDelete() {
  const path = document.getElementById('deletePath').value;
  if (!path) return showResponse('Please enter a DELETE path.');
  
  fetch(path, { method: 'DELETE' })
    .then(res => res.text())
    .then(text => showResponse(text))
    .catch(err => showResponse('Error: ' + err));
}

// CGI GET
function doCgiGet() {
  const path = document.getElementById('cgiPath').value;
  if (!path) return showResponse('Please enter CGI path.');
  
  fetch(path)
    .then(res => res.text())
    .then(text => showResponse(text))
    .catch(err => showResponse('Error: ' + err));
}

// CGI POST
function doCgiPost() {
  const path = document.getElementById('cgiPath').value;
  if (!path) return showResponse('Please enter CGI path.');
  
  fetch(path, { method: 'POST' })
    .then(res => res.text())
    .then(text => showResponse(text))
    .catch(err => showResponse('Error: ' + err));
}
