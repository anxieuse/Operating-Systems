#pragma once

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <zmq.hpp>

// send message to the particular socket
bool sendMessage(zmq::socket_t &socket, const std::string &message_string)
{
    // message size init
    zmq::message_t message(message_string.size());
    // message content init
    memcpy(message.data(), message_string.c_str(), message_string.size());
    return socket.send(message);
}

std::string receiveMessage(zmq::socket_t &socket)
{
    zmq::message_t message;
    int recResult;
    // receiving message from socket 
    try {
        recResult = (int)socket.recv(&message);
        if (recResult < 0) {
            perror("socket.recv()");
            exit(1);
        }
    }
    catch (...) {
        recResult = 0;
    }
    // transform to string
    std::string recieved_message((char *)message.data(), message.size());
    if (recieved_message.empty() || !recResult) {
        return "Error: Node is not available";
    }
    return recieved_message;
}

std::string getPortName(int port)
{
    return "tcp://127.0.0.1:" + std::to_string(port);
}
int bindSocket(zmq::socket_t &socket)
{
    int port = 8080;
    // create endpoint and bind it to the socket
    while (true) {
        try {
            socket.bind(getPortName(port));
            break;
        }
        catch (...) {
            port++;
        }
    }
    return port;
}

void createNode(int id, int port)
{
    // new node process
    execl("./child", "child", std::to_string(id).c_str(), std::to_string(port).c_str(), NULL);
}