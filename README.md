### 功能：
   c++实现获取主机的的内存大小，系统用户名，mac地址和硬盘序列号，然后封装成dll给go调用，并提供go调用的接口。[github传送门](https://github.com/easyAation/go-mac)
   ***GIT 地址  https://github.com/easyAation/go-mac***
### 运行截图:
![这里写图片描述](http://img.blog.csdn.net/20171110150953144?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQveW91cl9leWVzX3NlZV9zZWE=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
### c++部分：
  go语言获取mac地址这些不太方便，于是就思考可不可以调用c++封装的dll。这个demo用cgo来实现go对c++代码的调用。其中c++所爆露的接口都是`void`的，所需信息全部由指针做形参来获得，用为不知道怎么用go获取c++返回的`std::string`,所以就思考用传入指针的方式来获取想要的值，试了后发现是可以的。
### c++代码：


```
//simple.h:

/*
go查询主机硬盘序列号，内存大小，用户名，mac地址
qq:1034552569  欢迎大家一起交流学习
 */
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

extern "C" __declspec(dllexport) void get_Disk_number();
extern "C" __declspec(dllexport) void get_mac_address(char **macAdress);
extern "C" __declspec(dllexport) void get_user_name(char **name);
extern "C" __declspec(dllexport) void get_mem_size(char **_size);
extern "C" __declspec(dllexport) void get_disk_serial_number(char **number);
#endif // SIMPLE_H_INCLUDED

```

```
//simple.cpp
#include "simple.h"

void get_disk_serial_number(char **number)
{
    char         VolumeInfo[100];
    DWORD       VolNameSize=100;
    DWORD       VolSerialNumber;
    DWORD       VolMaxComponentLength;
    DWORD       VolFileSystemFlags;
    char         FileSystemNameBuffer[100];
    DWORD       FileSystemNameSize=100;
    GetVolumeInformation("c:\\",VolumeInfo,
                         VolNameSize,&VolSerialNumber,
                         &VolMaxComponentLength,&VolFileSystemFlags,
                         FileSystemNameBuffer,FileSystemNameSize);
    sprintf(*number , "%d" , VolSerialNumber);
  //  return VolSerialNumber;
}
void get_user_name(char **name)
{
    char str[1024];
    DWORD dwsize = 1024;
    GetUserName(str , &dwsize);
    strcpy(*name , str);
}
void get_mem_size(char **_size)
{
    MEMORYSTATUS stMem = {0};
    stMem.dwLength = sizeof stMem;
    GlobalMemoryStatus(&stMem);
    sprintf(*_size , "%d",  stMem.dwTotalPhys/(1024*1024));
   // return stMem.dwTotalPhys / (1024 * 1024);
}
void get_mac_address(char **macAdress) //有mac地址就返回第一个 ， 没有的话就返回空字符串。
{
    int i = 0;
    int ret = 0;
    unsigned long size = 0;

    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    ret = GetAdaptersInfo(pIpAdapterInfo,&size);

    if (ERROR_BUFFER_OVERFLOW == ret)
    {
        delete pIpAdapterInfo;
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[size];
        ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    }
    std::string s;
    bool flag = false;
    if (ERROR_SUCCESS == ret)
    {
        if(pIpAdapterInfo) //  每个网卡对应一个链表结点 , 只要第一个
        {
            char str[100];
            for (i = 0; i < pIpAdapterInfo->AddressLength; i++)
            {
                if (i < pIpAdapterInfo->AddressLength - 1)
                {
                    sprintf(str , "%02X-", pIpAdapterInfo->Address[i]);
                    s += str;
                }
                else
                {
                    sprintf(str , "%02X", pIpAdapterInfo->Address[i]);
                    s += str;
                }
            }
            flag = true;
            //std::cout<<s<<std::endl;
        }
    }

    if (pIpAdapterInfo)
    {
        delete pIpAdapterInfo;
        pIpAdapterInfo = NULL;
    }
    if(flag)
      strcpy(*macAdress , s.c_str());
    else
      strcpy(*macAdress , "");
}

```

ps:需要注意的是，如果这个代码编译不过，请检查一下你是否缺少了`iphlpapi.lib`这个动态链接库。

###go部分：
  我在go调用dll的时候遇到了一个很关于编码的问题，因为go语言的编码格式和c/c++是不同的，所以会出现汉字乱码的情况，这样传系统用户名的时候就麻烦了。通过一下代码发现：
  

```
//c++ demo
 char str[] = "连";
 for(int i=0; i<strlen(str); i++)
    printf("%d " , str[i]+256);

```
运行结果为: 193 172。

```go
//go demo
var ss string = "连"
data = []byte(ss)
fmt.Println(data)
```
运行结果：[232 191 158 ]。
####分析： 
   可以看到存储汉字的时候c++底层用了两个字节，而go用了三个字节，所以要解决这个问题就要进行编码转换。这里我用到了go中的一个库`github.com/axgle/mahonia`,它可以很方便的进行编码转换。
### go代码：

```
package main

/*
go查询主机硬盘序列号，内存大小，用户名，mac地址
qq:1034552569  欢迎大家一起交流学习
 */
import (
	"C"
	"fmt"
	"github.com/axgle/mahonia" //用于将gbk编码转化成go的utf-8编码
	"log"
	"syscall"
	"unsafe"
)
func init() {
	fmt.Println("------------------------------------------")
	handle, err := syscall.LoadDLL("Algorithm.dll")
	if err != nil {
		fmt.Println(err.Error())
		log.Fatal("init() faied.", err.Error())
	}

	fmt.Println(handle.Handle)
	fmt.Println(handle.Name)

	fmt.Println("------------函数加载成功------------------")

}

//返回操作系统当前的用户名
func Get_ser_name() (string, error) {

	handle, err := syscall.LoadDLL("Algorithm.dll")
	get_user_name, err := handle.FindProc("get_user_name")
	if err != nil {
		fmt.Println("get_user_name 不存在\n", err.Error())
		return "", err
	}

	str := make([]C.char, 1024)
	get_user_name.Call((uintptr)(unsafe.Pointer(&str)))

	var data []byte = []byte(C.GoString(&str[0]))

	/*
	 * 因为c++使用的编码格式与go不相符，会出现中文乱码问题
	 * 所以要进行转换
	 */
	enc := mahonia.NewDecoder("GBK")
	_, cdata, _ := enc.Translate(data, true)

	var name string = string(cdata[:])
	return name, nil
}

//获得主机内存大小 ， 返回string
func Get_mem_size() (string, error) {
	handle, err := syscall.LoadDLL("Algorithm.dll")
	get_mem_size, err := handle.FindProc("get_mem_size")
	if err != nil {
		fmt.Println("get_mem_size 不存在", err.Error())
		return "", err
	}

	str := make([]C.char, 1024)
	get_mem_size.Call((uintptr)(unsafe.Pointer(&str)))

	return string(C.GoString(&str[0])), nil
}
/*
 * 获取机器mac地址，如果有多个则返回第一个
 */
func Get_mac_address() (string, error) {
	handle, err := syscall.LoadDLL("Algorithm.dll")
	get_mac_address, err := handle.FindProc("get_mac_address")
	if err != nil {
		fmt.Println("get_mac_address 不存在", err.Error())
		return "", err
	}

	str := make([]C.char, 1024)
	get_mac_address.Call((uintptr)(unsafe.Pointer(&str)))
	return string(C.GoString(&str[0])), nil
}
func Get_disk_serial_numbere() (string, error) {
	handle, err := syscall.LoadDLL("Algorithm.dll")
	get_disk_serial_num, err := handle.FindProc("get_disk_serial_number")
	if err != nil {
		fmt.Println("get_disk_serial_num 不存在", err.Error())
		return "", err
	}

	str := make([]C.char, 1024)
	get_disk_serial_num.Call((uintptr)(unsafe.Pointer(&str)))
	return string(C.GoString(&str[0])), nil
}
func main() {
	name, err := Get_ser_name()
	if err != nil {
		log.Fatal(err.Error())
	}

	fmt.Println("name(gdk): ", name)

	size, err := Get_mem_size()
	if err != nil {
		log.Fatal(err.Error())
	}

	fmt.Println("mems size: ", size)

	address, err := Get_mac_address()
	if err != nil {
		log.Fatal(err.Error())
	}

	fmt.Println("address: ", address)

	disk_number, err := Get_disk_serial_numbere()
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("disk_number: ", disk_number)
}

```
###ps：
还有大家一个问题需要注意，如果go获取dll失败，很可能是因为你的go是64位的而dll是32位的。大家可以直接用我上面的c++代码编译成dll文件给go用，也可以用我已经编译好的[dll](https://github.com/easyAation/go-mac)用。
			
