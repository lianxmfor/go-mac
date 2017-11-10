#include "simple.h"
//欢迎大家技术交流QQ:1034552569
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
