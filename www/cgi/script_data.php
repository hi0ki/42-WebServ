#!/usr/bin/php
<?php
header("Content-Type: text/html");

// Read raw POST data from STDIN
$raw_post = file_get_contents('php://stdin');

// Parse it manually
parse_str($raw_post, $_POST);

echo "<pre>";
print_r($_POST);
echo "</pre>";
?>