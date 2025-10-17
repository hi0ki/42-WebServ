// Helper: display text in shared response box
function showResponse(text) {
  const box = document.getElementById('sharedResponse');
  box.textContent = text;
}

// ✅ Only GET redirects the page
function doGet() {
  const path = document.getElementById('getPath').value.trim();
  if (!path) return showResponse('Please enter a GET path.');
  // Real redirect
  window.location.href = path;
}

// POST Upload (no redirect)
async function doPost() {
  const formData = new FormData();
  const img = document.getElementById('imageFile').files[0];
  const vid = document.getElementById('videoFile').files[0];

  if (img) formData.append('image', img);
  if (vid) formData.append('video', vid);
  if (!img && !vid) return showResponse('Please select at least one file.');

  try {
    const res = await fetch('/upload', { method: 'POST', body: formData });
    const text = await res.text();
    showResponse(text);
  } catch (err) {
    showResponse('Error: ' + err);
  }
}

// Multipart POST (no redirect)
async function doMultipart() {
  const key = document.getElementById('mpKey').value.trim();
  const value = document.getElementById('mpValue').value.trim();

  if (!key || !value) return showResponse('Please fill both key and value.');

  const formData = new FormData();
  formData.append(key, value);

  try {
    const res = await fetch('/upload', { method: 'POST', body: formData });
    const text = await res.text();
    showResponse(text);
  } catch (err) {
    showResponse('Error: ' + err);
  }
}

// DELETE File (no redirect)
async function doDelete() {
  const path = document.getElementById('deletePath').value.trim();
  if (!path) return showResponse('Please enter a DELETE path.');

  try {
    const res = await fetch(path, { method: 'DELETE' });
    const text = await res.text();
    showResponse(text);
  } catch (err) {
    showResponse('Error: ' + err);
  }
}

// CGI GET (no redirect)
async function doCgiGet() {
  const path = document.getElementById('cgiPath').value.trim();
  if (!path) return showResponse('Please enter CGI path.');

  try {
    const res = await fetch(path);
    const text = await res.text();
    showResponse(text);
  } catch (err) {
    showResponse('Error: ' + err);
  }
}

// CGI POST (no redirect)
async function doCgiPost() {
  const path = document.getElementById('cgiPath').value.trim();
  if (!path) return showResponse('Please enter CGI path.');

  try {
    const res = await fetch(path, { method: 'POST' });
    const text = await res.text();
    showResponse(text);
  } catch (err) {
    showResponse('Error: ' + err);
  }
}
