#!/usr/bin/env python3
import os
import cgi

# Indique que la sortie est du HTML
print("Content-Type: text/html\n")

# Récupération des variables d'environnement CGI
method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
query = os.environ.get("QUERY_STRING", "")
script_path = os.environ.get("SCRIPT_FILENAME", "")
path_info = os.environ.get("PATH_INFO", "")

# Pour parser les paramètres GET ou POST
form = cgi.FieldStorage()
name = form.getvalue("name", "inconnu")
city = form.getvalue("city", "inconnu")

# Construction de la page HTML
print("<html>")
print("<head><title>CGI Example</title></head>")
print("<body>")
print(f"<h1>CGI Script Test</h1>")
print(f"<p>Méthode : {method}</p>")
print(f"<p>Query string : {query}</p>")
print(f"<p>Script path : {script_path}</p>")
print(f"<p>Path info : {path_info}</p>")
print(f"<p>Name : {name}</p>")
print(f"<p>City : {city}</p>")
print("</body></html>")
