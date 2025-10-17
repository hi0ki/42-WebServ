async function doGet() {
  const path = document.getElementById('getPath').value;
  if (!path) return alert('Enter a path!');
  window.location.href = path; // redirect after GET
}

async function doPost() {
  const formData = new FormData();
  const img = document.getElementById('imageFile').files[0];
  const vid = document.getElementById('videoFile').files[0];
  if (img) formData.append('image', img);
  if (vid) formData.append('video', vid);

  const res = await fetch('/upload', { method: 'POST', body: formData });
  document.getElementById('postResponse').textContent = await res.text();
  window.location.href = '/';
}

async function doMultipart() {
  const key = document.getElementById('mpKey').value;
  const value = document.getElementById('mpValue').value;
  if (!key || !value) return alert('Enter key and value!');
  const formData = new FormData();
  formData.append(key, value);
  const res = await fetch('/upload', { method: 'POST', body: formData });
  document.getElementById('mpResponse').textContent = await res.text();
}

async function doDelete() {
  const path = document.getElementById('deletePath').value;
  if (!path) return alert('Enter path to delete!');
  const res = await fetch(path, { method: 'DELETE' });
  document.getElementById('deleteResponse').textContent = await res.text();
}

async function doCgiGet() {
  const path = document.getElementById('cgiPath').value;
  if (!path) return alert('Enter CGI path!');
  const res = await fetch(path);
  document.getElementById('cgiResponse').textContent = await res.text();
}

async function doCgiPost() {
  const path = document.getElementById('cgiPath').value;
  if (!path) return alert('Enter CGI path!');
  const res = await fetch(path, { method: 'POST' });
  document.getElementById('cgiResponse').textContent = await res.text();
}
