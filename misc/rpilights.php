<?php

$id = $_GET["id"];

shell_exec("/usr/local/bin/rpilights $id > /dev/null 2>/dev/null");

$status = shell_exec("/usr/local/bin/rpilights status");

$status =  str_replace(array("\r", "\n"), '', $status);

$command = file_get_contents("/home/pi/rpilights/.command");

$command =  str_replace(array("\r", "\n"), '', $command);

if (strpos($status, 'are on') !== false) {
	echo strtoupper("$command ON");
} else {
	echo strtoupper("$command OFF");
}

?>
