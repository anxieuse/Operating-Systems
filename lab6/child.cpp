#include "server.hpp"
#include <csignal>
#include <string>
#include <unistd.h>

int main(int argc, char **argv)
{
    // айди и номер порта, к к-рым нужно подключиться
    int id = std::stoi(argv[1]);
    int parentPort = std::stoi(argv[2]);

    // подключение
    zmq::context_t context(2);
    zmq::socket_t parentSocket(context, ZMQ_REP);

    parentSocket.connect(getPortName(parentPort));

    zmq::socket_t leftSocket(context, ZMQ_REQ);
    zmq::socket_t rightSocket(context, ZMQ_REQ);

    int linger = 0;
    leftSocket.setsockopt(ZMQ_SNDTIMEO, 2000);
    leftSocket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    rightSocket.setsockopt(ZMQ_SNDTIMEO, 2000);
    rightSocket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    int leftPort = bindSocket(leftSocket);
    int rightPort = bindSocket(rightSocket);

    // вспомогательные переменные
    int leftPid = 0;
    int rightPid = 0;
    int leftId = 0;
    int rightId = 0;

    std::string request;
    std::string cmd;

    while (true) {
        request = receiveMessage(parentSocket);
        std::istringstream cmdStream(request);
        cmdStream >> cmd;
        if (cmd == "id") {
            printf("debug\n");
            std::string parentString = "OK:" + std::to_string(id);
            sendMessage(parentSocket, parentString);
        }
        else if (cmd == "pid") {
            std::string parentString = "OK:" + std::to_string(getpid());
            sendMessage(parentSocket, parentString);
        }
        else if (cmd == "create") {
            int idToCreate;
            cmdStream >> idToCreate;
            if (idToCreate == id) {
                std::string msgString = "Error: Already exists";
                sendMessage(parentSocket, msgString);
            }
            else if (idToCreate < id) {
                if (leftPid == 0) {
                    leftPid = fork();
                    if (leftPid == -1) {
                        sendMessage(parentSocket, "Error: fork fails");
                        leftPid = 0;
                    }
                    else if (leftPid == 0) {
                        createNode(idToCreate, leftPort);
                    }
                    else {
                        leftId = idToCreate;
                        sendMessage(leftSocket, "pid");
                        sendMessage(parentSocket, receiveMessage(leftSocket));
                    }
                }
                else {
                    sendMessage(leftSocket, request);
                    sendMessage(parentSocket, receiveMessage(leftSocket));
                }
            }
            else {
                if (rightPid == 0) {
                    rightPid = fork();
                    if (rightPid == -1) {
                        sendMessage(parentSocket, "Error: fork fails");
                        rightPid = 0;
                    }
                    else if (rightPid == 0) {
                        createNode(idToCreate, rightPort);
                    }
                    else {
                        rightId = idToCreate;
                        sendMessage(rightSocket, "pid");
                        sendMessage(parentSocket, receiveMessage(rightSocket));
                    }
                }
                else {
                    sendMessage(rightSocket, request);
                    sendMessage(parentSocket, receiveMessage(rightSocket));
                }
            }
        }
        else if (cmd == "remove") {
            int idToDelete;
            cmdStream >> idToDelete;
            if (idToDelete < id) {
                if (leftId == 0) {
                    sendMessage(parentSocket, "Error: Node not found");
                }
                else if (leftId == idToDelete) {
                    sendMessage(leftSocket, "recursiveKilling");
                    receiveMessage(leftSocket);
                    kill(leftPid, SIGTERM);
                    kill(leftPid, SIGKILL);
                    leftId = 0;
                    leftPid = 0;
                    sendMessage(parentSocket, "OK");
                }
                else {
                    sendMessage(leftSocket, request);
                    sendMessage(parentSocket, receiveMessage(leftSocket));
                }
            }
            else {
                if (rightId == 0) {
                    sendMessage(parentSocket, "Error: Node not found");
                }
                else if (rightId == idToDelete) {
                    sendMessage(rightSocket, "recursiveKilling");
                    receiveMessage(rightSocket);
                    kill(rightPid, SIGTERM);
                    kill(rightPid, SIGKILL);
                    rightId = 0;
                    rightPid = 0;
                    sendMessage(parentSocket, "OK");
                }
                else {
                    sendMessage(rightSocket, request);
                    sendMessage(parentSocket, receiveMessage(rightSocket));
                }
            }
        }
        else if (cmd == "exec") {
            int execNodeId;
            cmdStream >> execNodeId;
            if (execNodeId == id) {
                std::string receiveMessage = "Node is available";
                sendMessage(parentSocket, receiveMessage);
            }
            else if (execNodeId < id) {
                if (leftPid == 0) {
                    std::string receiveMessage = "Error:" + std::to_string(execNodeId) + ": Not found";
                    sendMessage(parentSocket, receiveMessage);
                }
                else {
                    sendMessage(leftSocket, request);
                    sendMessage(parentSocket, receiveMessage(leftSocket));
                }
            }
            else {
                if (rightPid == 0) {
                    std::string receiveMessage = "Error:" + std::to_string(execNodeId) + ": Not found";
                    sendMessage(parentSocket, receiveMessage);
                }
                else {
                    sendMessage(rightSocket, request);
                    sendMessage(parentSocket, receiveMessage(rightSocket));
                }
            }
        }
        else if (cmd == "pingall") {
            std::ostringstream res;
            std::string leftRes;
            std::string rightRes;
            res << id << " ";
            if (leftPid != 0) {
                sendMessage(leftSocket, "pingall");
                leftRes = receiveMessage(leftSocket);
            }
            if (rightPid != 0) {
                sendMessage(rightSocket, "pingall");
                rightRes = receiveMessage(rightSocket);
            }
            if (!leftRes.empty() && leftRes.substr(0, 5) != "Error") {
                res << leftRes << " ";
            }
            if (!rightRes.empty() && rightRes.substr(0, 5) != "Error") {
                res << rightRes << " ";
            }
            sendMessage(parentSocket, res.str());
        }
        else if (cmd == "recursiveKilling") { 
            if (leftPid == 0 && rightPid == 0) {
                sendMessage(parentSocket, "OK");
            }
            else {
                if (leftPid != 0) {
                    sendMessage(leftSocket, "recursiveKilling");
                    receiveMessage(leftSocket);
                    kill(leftPid, SIGTERM);
                    kill(leftPid, SIGKILL);
                }
                if (rightPid != 0) {
                    sendMessage(rightSocket, "recursiveKilling");
                    receiveMessage(rightSocket);
                    kill(rightPid, SIGTERM);
                    kill(rightPid, SIGKILL);
                }
                sendMessage(parentSocket, "OK");
            }
        }
        if (parentPort == 0) {
            break;
        }
    }
}
