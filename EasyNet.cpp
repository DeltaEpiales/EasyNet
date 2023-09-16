#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>

using namespace std;

// Function to check if a port is open
bool isPortOpen(const char* ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return false;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &(server.sin_addr));

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
        close(sock);
        return true;
    }

    close(sock);
    return false;
}

// Function to scan a range of ports for a single IP address
void scanPorts(const char* ip, int startPort, int endPort) {
    for (int port = startPort; port <= endPort; ++port) {
        if (isPortOpen(ip, port)) {
            cout << "Port " << port << " is open on " << ip << endl;
        }
    }
}

int main() {
    const char* targetIP = "127.0.0.1"; // Change this to the target IP address
    int startPort = 1;                 // Starting port number
    int endPort = 1024;                // Ending port number
    int numThreads = 4;                // Number of threads for parallel scanning

    vector<thread> threads;

    // Split the range of ports into equal parts for each thread
    int portsPerThread = (endPort - startPort + 1) / numThreads;
    int remainingPorts = (endPort - startPort + 1) % numThreads;

    int currentStartPort = startPort;
    int currentEndPort = currentStartPort + portsPerThread - 1;

    // Create threads for parallel port scanning
    for (int i = 0; i < numThreads; ++i) {
        if (i == numThreads - 1) {
            // Add any remaining ports to the last thread
            currentEndPort += remainingPorts;
        }

        threads.emplace_back(scanPorts, targetIP, currentStartPort, currentEndPort);

        currentStartPort = currentEndPort + 1;
        currentEndPort = currentStartPort + portsPerThread - 1;
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
