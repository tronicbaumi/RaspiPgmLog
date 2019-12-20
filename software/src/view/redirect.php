<?php
/**
 * start the NodeJs server for the data logger and redirect to its website
 */

exec('bash -c "exec nohup setsid node "./socket/socket_server.js" > /dev/null 2>&1 &"');
header("Location: http://" . $_SERVER['SERVER_ADDR'] . ":3000");
exit;