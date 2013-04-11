gcc -o aa.exe main.cpp webserver.cpp urlHelper.cpp base64.cpp response.cpp -lstdc++ -lws2_32

gcc -g -o _req.exe _req.cpp base64.cpp -lws2_32 -lstdc++
