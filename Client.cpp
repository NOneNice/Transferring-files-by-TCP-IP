#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <fstream>
#include <experimental/filesystem>
#include <Windows.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning (disable: 4996)

using namespace std;

void recv_file(SOCKET* sock) {

	char file_size_str[16]; // Так как принимаем в чарах, то и файл надо сделать строкой
	char file_name[32];

	recv(*sock, file_size_str, 16, 0); // Тут указываем максимальное длина в байтах файла может быть 9999999999999999
	int file_size = atoi(file_size_str); // И как только мы приняли файл, то переводим из строки в инт
	char* bytes = new char[file_size]; // Для принятия самого файла в байтак

	recv(*sock, file_name, 32, 0);

	fstream file;
	file.open(file_name, ios_base::out | ios_base::binary);

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "size: " << file_size << endl;
	cout << "name: " << file_name << endl;

	if (file.is_open()) {

		recv(*sock, bytes, file_size, 0);
		cout << "data: " << bytes << endl;
		cout << "Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << endl;

		file.write(bytes, file_size);
		cout << "Save\n";
	}
	else
		cout << "Error file open\n";

	delete[] bytes;
	file.close();

}


int main(int argc, char* argv[]) {
	
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(80);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) == 0) { // Тут точно так же соеденяемся с клиентом по выше указаным данным
		cout << "Connectied!\n";
		recv_file(&Connection);
	}
	

	system("pause");
	return 0;
}