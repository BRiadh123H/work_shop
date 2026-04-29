#!/bin/bash
# Script pour tester les serveurs et clients
cd /mnt/c/Users/riadb/Downloads/tpreseau

echo "=== TEST UDP ==="
./Serveur\ udp.out 8889 > server_udp.log 2>&1 &
SERVER_PID=$!
sleep 1
./Client\ udp.out 127.0.0.1 8889 | head -n 10
kill $SERVER_PID
echo "Check server_udp.log for server output."
echo -e "\n"

echo "=== TEST TCP ==="
./Serveur\ tcp.out 8888 > server_tcp.log 2>&1 &
SERVER_PID=$!
sleep 1
./Client\ tcp.out 127.0.0.1 8888 | head -n 5
kill $SERVER_PID
echo "Check server_tcp.log for server output."
echo -e "\n"

echo "=== TEST HTTP CLIENT (Google) ==="
echo -e "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n" | ./client_http.out www.google.com 80 | head -n 15
