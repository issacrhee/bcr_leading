#include <iostream>
#include <iomanip>
#include <cstring>
#include <arpa/inet.h> // For htonl and ntohl
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

const char *IP_ADDR = "192.168.1.181";
const int LISTEN_PORT = 4001;
const int SENSOR_UDP_PORT = 8890;

int s; // socket file descriptor

// Function to unpack a float from a byte array
float unpackFloat(const char *bytes)
{
    uint32_t asInt = 0;
    std::memcpy(&asInt, bytes, sizeof(asInt));
    asInt = ntohl(asInt); // Convert from network byte order to host byte order
    float result;
    std::memcpy(&result, &asInt, sizeof(result));
    return result;
}

// Function to receive message
char *recvMsg()
{
    static char recvData[50];
    struct sockaddr_in from;
    socklen_t fromLen = sizeof(from);

    ssize_t bytesReceived = recvfrom(s, recvData, sizeof(recvData), 0, (struct sockaddr *)&from, &fromLen);
    if (bytesReceived != sizeof(recvData))
    {
        std::cout << bytesReceived << std::endl;
        std::cout << sizeof(recvData) << std::endl;
        std::cerr << "Received incorrect number of bytes" << std::endl;
        return nullptr;
    }

    return recvData;
}

int main()
{
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    struct sockaddr_in sensorAddr;
    sensorAddr.sin_family = AF_INET;
    sensorAddr.sin_port = htons(SENSOR_UDP_PORT);
    inet_pton(AF_INET, IP_ADDR, &sensorAddr.sin_addr);

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    std::string sendData = "000302"; /////// Initialize function for sensor data will be updated
    std::cout << "Sensor IP: " << IP_ADDR << std::endl;
    std::cout << "Sensor UDP PORT: " << SENSOR_UDP_PORT << std::endl;
    std::cout << sendData << std::endl;

    std::vector<unsigned char> sendDataBytes(sendData.size() / 2);
    for (size_t i = 0; i < sendData.size(); i += 2)
    {
        sendDataBytes[i / 2] = std::stoi(sendData.substr(i, 2), nullptr, 16);
    }
    std::cout << std::hex << std::setfill('0');
    for (const auto &byte : sendDataBytes)
    {
        std::cout << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    sendto(s, sendDataBytes.data(), sendDataBytes.size(), 0, (struct sockaddr *)&sensorAddr, sizeof(sensorAddr));
    char *recvData = recvMsg();

    if (!recvData)
    {
        std::cerr << "Failed to receive data" << std::endl;
        close(s);
        return 1;
    }

    // Initializing
    float Fx = 0, Fy = 0, Fz = 0;
    float Tx = 0, Ty = 0, Tz = 0;
    float Temp = 0;

    for (int i = 0; i < 10000; ++i)
    {
        recvData = recvMsg();
        if (!recvData)
        {
            break;
        }

        Fx = unpackFloat(recvData);
        Fy = unpackFloat(recvData + 4);
        Fz = unpackFloat(recvData + 8);
        Tx = unpackFloat(recvData + 12);
        Ty = unpackFloat(recvData + 16);
        Tz = unpackFloat(recvData + 20);
        Temp = unpackFloat(recvData + 48);

        std::cout << std::fixed << std::setprecision(2)
                  << "Fx: " << Fx << " "
                  << "Fy: " << Fy << " "
                  << "Fz: " << Fz << " "
                  << "Tx: " << Tx << " "
                  << "Ty: " << Ty << " "
                  << "Tz: " << Tz << " "
                  << "Temp: " << Temp << " "
                  << std::endl;

        std::vector<std::string> packet_aidin = {
            std::to_string(Fx),
            std::to_string(Fy),
            std::to_string(Fz),
            std::to_string(Tx),
            std::to_string(Ty),
            std::to_string(Tz),
            std::to_string(Temp)};
    }

    close(s);
    return 0;
}