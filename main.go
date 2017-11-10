package main

import (
	"C"
	"fmt"
	"github.com/axgle/mahonia"
	"log"
	"syscall"
	"unsafe"
)

var (
	handle *syscall.DLL
)

func init() {
	fmt.Println("-------------------------------------------")
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
