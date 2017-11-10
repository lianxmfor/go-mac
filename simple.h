//Algorithm.h
#ifndef SIMPLE_H_INCLUDED
#define SIMPLE_H_INCLUDED

#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif // __cplusplus

#include <windows.h>
#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <algorithm>
#include <iostream>
#pragma comment(lib, "iphlpapi.lib")
extern "C" __declspec(dllexport) void get_mac_address(char **macAdress);
extern "C" __declspec(dllexport) void get_user_name(char **name);
extern "C" __declspec(dllexport) void get_mem_size(char **_size);
extern "C" __declspec(dllexport) void get_disk_serial_number(char **number);
#endif // SIMPLE_H_INCLUDED
