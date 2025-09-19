async function getData() {
    try {
      const response = await fetch('get_page.html');
      const text = await response.text();
      document.getElementById('getResult').textContent = text;
    } catch (err) {
      document.getElementById('getResult').textContent = 'Error: ' + err;
    }
  }
  
  document.getElementById('postForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    const data = document.getElementById('postInput').value;
  
    try {
      const response = await fetch('/test-post', {
        method: 'POST',
        headers: { 'Content-Type': 'text/plain' },
        body: data
      });
      const text = await response.text();
      document.getElementById('postResult').textContent = text;
    } catch (err) {
      document.getElementById('postResult').textContent = 'Error: ' + err;
    }
  });
  
  async function deleteData() {
    try {
      const response = await fetch('/test-delete', { method: 'DELETE' });
      const text = await response.text();
      document.getElementById('deleteResult').textContent = text;
    } catch (err) {
      document.getElementById('deleteResult').textContent = 'Error: ' + err;
    }
  }
  
  document.getElementById('uploadForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    const fileInput = document.getElementById('uploadInput');
    const formData = new FormData();
    formData.append('image', fileInput.files[0]);
  
    try {
      const response = await fetch('/upload', {
        method: 'POST',
        body: formData
      });
      const text = await response.text();
      document.getElementById('uploadResult').textContent = text;
    } catch (err) {
      document.getElementById('uploadResult').textContent = 'Error: ' + err;
    }
  });