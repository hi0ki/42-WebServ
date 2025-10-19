# Webserv

> *"This is when you finally understand why URLs start with HTTP"*

A custom HTTP/1.1 web server written in C++98, inspired by NGINX. Serves static websites, handles file uploads, executes CGI scripts, and manages multiple concurrent connections with non-blocking I/O.

## ✨ Features

- HTTP methods: GET, POST, DELETE
- Static file serving with configurable directory listing
- File upload with size limits
- CGI execution (Python, PHP)
- HTTP redirections
- Custom error pages
- Multiple server blocks (virtual hosts)
- Non-blocking I/O with `poll()`/`select()`/`kqueue()`/`epoll()`

## 🛠️ Tech Stack

**Language:** C++98 | **Build:** Makefile | **I/O:** Non-blocking sockets

## 📦 Installation

```bash
git clone https://github.com/yourusername/webserv.git
cd webserv
make
```

**Makefile targets:** `all`, `clean`, `fclean`, `re`

## 🚀 Usage

```bash
./webserv [config_file]
```

Access via browser: `http://localhost:8080`

### Configuration Example

```nginx
server {
    listen 127.0.0.1:8080;
    server_name mysite.com;
    
    root /;
    autoindex off;
    index test.html;
    methods POST GET DELETE;
    error_page 404 /errors/404.html;

    location / {
        root /;
        index file.html;
        methods POST GET DELETE;
    }

    location /cgi {
        root /;
        cgi_enabled on;
        cgi_extension .py .php;
        cgi_path /usr/bin/python3 /usr/bin/php;
        methods POST GET DELETE;
    }

    location /upload {
        root /;
        methods GET DELETE POST;
        max_upload_size 500;
    }

    location /old {
        return 301 /new;
    }
}
```

**Configuration Options:**
- `listen` - IP:port binding
- `server_name` - Virtual host name
- `root` - Document root directory
- `autoindex` - Enable/disable directory listing
- `index` - Default file to serve
- `methods` - Allowed HTTP methods
- `error_page` - Custom error pages
- `cgi_enabled` - Enable CGI execution
- `cgi_extension` - CGI file extensions
- `cgi_path` - Path to CGI interpreters
- `max_upload_size` - Upload size limit (MB)
- `return` - HTTP redirection

