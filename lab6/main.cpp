#include "server.hpp"
#include "tree.hpp"
#include <algorithm>
#include <csignal>
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>
#include <zmq.hpp>

int main()
{
    zmq::context_t context(1);
    zmq::socket_t mainSocket(context, ZMQ_REQ);

    // ZMQ_SNDTIMEO: Maximum time before a send operation returns with EAGAIN
    mainSocket.setsockopt(ZMQ_SNDTIMEO, 2000);
    // ZMQ_LINGER: linger period determines how long pending messages which have yet to be sent to a peer shall linger in memory after a socket is closed
    int linger = 0;
    mainSocket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    int port = bindSocket(mainSocket);

    Tree tree;

    int childPid = 0;
    int childId = 0;
    int createNodeId;

    int id;
    char excmd;
    std::string word;
    int val;

    std::string sendingMsg;
    std::string receivedMsg;

    std::string cmd;
    while (std::cout << "> " && std::cin >> cmd) {
        if (cmd == "create") {
            std::cin >> createNodeId;
            if (childPid == 0) {
                childPid = fork();
                if (childPid == -1) {
                    std::cout << "Error: fork fails\n";
                    childPid = 0;
                    exit(1);
                }
                else if (childPid == 0) {
                    createNode(createNodeId, port);
                }
                else {
                    childId = createNodeId;
                    sendMessage(mainSocket, "pid");
                    receivedMsg = receiveMessage(mainSocket);
                }
            }
            else {
                std::ostringstream sendingMsgStream;
                sendingMsgStream << "create " << createNodeId;
                sendMessage(mainSocket, sendingMsgStream.str());
                receivedMsg = receiveMessage(mainSocket);
            }

            if (receivedMsg.substr(0, 2) == "OK") {
                tree.insert(createNodeId);
            }

            std::cout << receivedMsg << "\n";
        }
        else if (cmd == "remove") {
            if (childPid == 0) {
                std::cout << "Error: Not found\n";
                continue;
            }
            std::cin >> createNodeId;
            if (createNodeId == childId) {
                kill(childPid, SIGTERM);
                kill(childPid, SIGKILL);
                childId = childPid = 0;
                std::cout << "OK\n";
                tree.erase(createNodeId);
                continue;
            }
            sendingMsg = "remove " + std::to_string(createNodeId);
            sendMessage(mainSocket, sendingMsg);
            receivedMsg = receiveMessage(mainSocket);
            if (receivedMsg.substr(0, 2) == "OK")
                tree.erase(createNodeId);
            std::cout << receivedMsg << "\n";
        }
        else if (cmd == "exec") {
            std::cin >> id >> excmd >> word;
            if (excmd == '+')
                std::cin >> val;

            sendingMsg = "exec " + std::to_string(id);
            sendMessage(mainSocket, sendingMsg);

            receivedMsg = receiveMessage(mainSocket);
            if (receivedMsg == "Node is available") {
                if (excmd == '+') {
                    tree.dictInsertWord(id, word, val);
                    std::cout << "OK:" << id << std::endl;
                }
                if (excmd == '?') {
                    std::cout << "OK:" << id << ": ";
                    tree.dictGetWord(id, word);
                }
            }
            else {
                std::cout << receivedMsg << std::endl;
            }
        }
        else if (cmd == "ping") {
            std::cin >> id;
            std::vector<int> nodesList = tree.getNodesList();
            bool nodeExists = binary_search(nodesList.begin(), nodesList.end(), id);
            if (nodeExists == 0) {
                std::cout << "Error: Not found\n";
            }
            else {
                sendMessage(mainSocket, "exec " + std::to_string(id));
                receivedMsg = receiveMessage(mainSocket);
                std::istringstream is;
                if (receivedMsg.substr(0, 5) == "Error")
                    std::cout << "OK:0\n";
                else
                    std::cout << "OK:1\n";
            }
        }
        else if (cmd == "pingall") {
            std::vector<int> nodesList = tree.getNodesList();
            if (nodesList.empty()) {
                std::cout << "Error: Tree is empty\n";
                continue;
            }

            sendMessage(mainSocket, "pingall");
            receivedMsg = receiveMessage(mainSocket);
            std::istringstream is;
            if (receivedMsg.substr(0, 5) == "Error")
                is = std::istringstream("");
            else
                is = std::istringstream(receivedMsg);

            std::set<int> receivedNodes;
            int rec_id;
            while (is >> rec_id) {
                receivedNodes.insert(rec_id);
            }

            std::cout << "Received nodes: ";
            for (const int &i : receivedNodes)
                std::cout << i << " ";

            std::cout << "\nNodes list:     ";
            for (const int &i : nodesList)
                std::cout << i << " ";

            std::cout << "\n";
        }
        else if (cmd == "exit") {
            break;
        }
        else {
            std::cout << "Unknown command\n";
        }
    }
    return 0;
}
