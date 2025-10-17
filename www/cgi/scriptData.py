#!/usr/bin/python3
import sys
import urllib.parse

# Print HTTP header (like PHP's header())
print("Content-Type: text/html")
print()  # Empty line to separate headers from body

# Read raw POST data from STDIN (like PHP's file_get_contents('php://stdin'))
raw_post = sys.stdin.read()

# Parse it manually (like PHP's parse_str())
post_data = {}
if raw_post:
    post_data = dict(urllib.parse.parse_qsl(raw_post))

# Output like PHP's print_r($_POST)
print("<pre>")
if post_data:
    print("Array")
    print("(")
    for key, value in post_data.items():
        print(f"    [{key}] => {value}")
    print(")")
else:
    print("Array")
    print("(")
    print(")")
print("</pre>")