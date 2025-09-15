async function getData() {
    try {
      const response = await fetch('/test-get');
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
  