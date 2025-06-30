//
// Created by dingjing on 25-6-30.
//

#ifndef peony_plugins_COMMON_H
#define peony_plugins_COMMON_H
#include "ipc.h"

// 右键菜单手动加密
bool        get_mouse_manual_encrypt    ();
bool        get_privileged_decrypt      ();
bool        check_is_encrypt_file       (const char* file);

void        send_data_to_daemon         (IpcServerType type, short isCN, char* files, unsigned int dataLen);
int         send_data_to_daemon         (IpcServerType type, const char* sendData, int sendDataLen, char* recvData, int recvDataBufLen);
int         send_data_with_return_int   (IpcServerType type, const char* sendData, int sendDataLen);

#endif // peony_plugins_COMMON_H
