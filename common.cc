//
// Created by dingjing on 25-6-30.
//

#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>


#define STR_FREE(x) do { if (x) { free (x); x = NULL; } } while (0)

void send_data_to_daemon(IpcServerType type, short isCN, char* files, unsigned int dataLen)
{
    int val = 1;
    int sockFd = 0;
    int timeout = 2000;
    int revTimeout = 31 * 1000;
    struct sockaddr_un address;

    if ((sockFd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("socket() failed\n");
        return;
    }

    setsockopt(sockFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &revTimeout, sizeof(revTimeout));
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, 108, IPC_SERVER_SOCKET_PATH);
    const int len = sizeof(address);

    if (0 != connect(sockFd, reinterpret_cast<struct sockaddr*>(&address), len)) {
        printf("connect error\n");
        return;
    }

    struct IpcMessage msgBuf = {};
    struct PrivilegedDecAndEncData data = {};

    data.isCN = isCN;
    data.isPierec = 0;
    data.filesLen = dataLen;

    msgBuf.type = type;
    msgBuf.dataLen = sizeof(data) + dataLen + 1;

    const int allLen = dataLen + sizeof(data) + sizeof(msgBuf);
    auto sendBuf = static_cast<char*>(malloc(allLen));
    if (sendBuf) {
        memset(sendBuf, 0, allLen);
        memcpy(sendBuf, &msgBuf, sizeof(msgBuf));
        memcpy(sendBuf + sizeof(msgBuf), &data, sizeof(data));
        memcpy(sendBuf + sizeof(msgBuf) + sizeof(data), files, dataLen);
        write(sockFd, sendBuf, allLen);
        free(sendBuf);
        sendBuf = nullptr;
    }

    close(sockFd);
}

bool get_mouse_manual_encrypt()
{
    return (0 == send_data_with_return_int(IPC_TYPE_CONFIG_MOUSE_MANUAL_ENCRYPT, NULL, 0));
}

bool get_privileged_decrypt()
{
    return (0 == access("/usr/local/andsec/__andsec_menu_decrypt", F_OK));
}

bool check_is_encrypt_file(const char* file)
{
    DataInfo di;

    di.dataLen = strlen(file);
    int len = sizeof(DataInfo) + strlen(file) + 1;
    char* buffer = (char*) malloc (len);
    memset(buffer, 0, len);
    memccpy(buffer, &di, 1, sizeof(di));
    memccpy(buffer + sizeof(di), file, 1, strlen(file));

    const int ret = send_data_with_return_int(IPC_TYPE_TEST_ENCRYPT_FILE_SYNC, buffer, len);
    STR_FREE(buffer);

    return (1 == ret);
}

int send_data_to_daemon(IpcServerType type, const char* sendData, int sendDataLen, char* recvData, int recvDataBufLen)
{
    int res = 0;
    int val = 1;
    int sockFd = 0;
    int timeout = 2000;
    int revtimeout = 31 * 1000;
    struct sockaddr_un address;

    char* sendBuf = NULL;
    char recvBuf[1024] = {0};

    if (sendDataLen < 0 || recvDataBufLen < 0) {
        syslog(LOG_ERR, "sendData: %p, recvData: %p, sendDataLen: %d, recvDataBufLen: %d", sendData, recvData, sendDataLen, recvDataBufLen);
        return -1;
    }

    do {
        if ((sockFd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
            syslog(LOG_ERR, "[IPC] socket() failed!");
            res = -1;
            break;
        }

        setsockopt(sockFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &revtimeout, sizeof(revtimeout));
        setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

        memset(&address, 0, sizeof(struct sockaddr_un));
        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, 108, IPC_SERVER_SOCKET_PATH);
        int len = sizeof(address);

        if (0 != connect(sockFd, (struct sockaddr*)&address, len)) {
            syslog(LOG_ERR, "[IPC] connect error!");
            res = -1;
            break;
        }

        struct IpcMessage msgBuf;
        memset(&msgBuf, 0, sizeof(msgBuf));

        msgBuf.type = type;
        msgBuf.dataLen = sendDataLen;

        // 开始发送
        int allLen = (int) sizeof(struct IpcMessage) + sendDataLen;
        sendBuf = (char*) malloc (sizeof(char) * allLen);
        if (!sendBuf) {
            syslog(LOG_ERR, "[IPC] malloc is null!");
            res = -1;
            break;
        }
        memset(sendBuf, 0, allLen);
        memcpy(sendBuf, &msgBuf, sizeof(msgBuf));
        if (sendDataLen > 0) {
            memcpy(sendBuf + sizeof(msgBuf), sendData, sendDataLen);
        }
        write(sockFd, sendBuf, allLen);
        STR_FREE(sendBuf);

        // 开始接收
        if (recvData) {
            int ri = 0;

            ri = read(sockFd, recvBuf, sizeof(recvBuf));
            if (ri < sizeof(struct IpcMessage)) {
                res = -1;
                syslog(LOG_ERR, "[IPC] recvBuf len too small.");
            }
            else {
                struct IpcMessage* im = (struct IpcMessage*) recvBuf;
                memset(recvData, 0, recvDataBufLen);
                memcpy(recvData, im->data, im->dataLen);
            }
        }
    } while (0);

    STR_FREE(sendBuf);

    return res;
}

int send_data_with_return_int(IpcServerType type, const char* sendData, int sendDataLen)
{
    int res = -1;

    char buf[8] = {0};

    if (0 == send_data_to_daemon(type, sendData, sendDataLen, buf, sizeof(buf) - 1)) {
        res = (int) strtol(buf, NULL, 10);
    }

    printf("control ret: %s\n", buf);

    return res;
}
