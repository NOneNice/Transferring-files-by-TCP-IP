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


void send_file(SOCKET* sock, const string& file_name) {

	fstream file; //Открываем файл
	file.open(file_name, ios_base::in | ios_base::binary);

	if (file.is_open()) {
		
		int file_size = experimental::filesystem::file_size(file_name) + 1; //Узнаем вес файла
		
		char* bytes = new char[file_size]; // Выделяем масив для файла

		file.read(bytes, file_size); // Читаем файл

		SYSTEMTIME st;
		GetLocalTime(&st);

		cout << "size:" << file_size << endl;
		cout << "name:" << file_name << endl;
		cout << "data:" << bytes << endl;
		cout << "Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << endl;

		send(*sock, to_string(file_size).c_str(), 16, 0); 
		send(*sock, file_name.c_str(), 32, 0);
		send(*sock, bytes, file_size, 0);
		delete[] bytes;
	}
	else
		cout << "Error file open\n";
		
		file.close();
}


int main(int argc, char* argv[]) {
	
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1); // Запрашиваем библиотеку
	if (WSAStartup(DLLVersion, &wsaData) != 0) { // Загружаем библеотеку 1: Запрашиваемая версия библеотки 2: Ссылка на структуру
		cout << "Error" << endl; // Если библеотека загрузилась то мы перейдем дальше, если же нет то выйдем из функции мейн и выводим ошибку 
		exit(1);
	}

		SOCKADDR_IN addr; // Структура для хранения адреса 
		int sizofaddr = sizeof(addr); 
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // индефекация адреса
		addr.sin_port = htons(80); // Индефикация порта
		addr.sin_family = AF_INET; // Семейство протоколов
		// Что бы два компьютера смогли сооединиться надо что бы 1 слушал а другой принимал прослушку
		SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);  // Тут обьявляем что мы будем соеденятся через интерент протоколы и что структура Slisten будет слушать
		bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // Привязываем адрес к сокету, указываем на структуру SOCKADDER, размер структуры сокадер
		listen(sListen, SOMAXCONN); // Тут будем слушать порт, и ожидать подключение 

		SOCKET nemConnection; // Новый сокет что бы удерживать соеденение с клиентом 
		nemConnection = accept(sListen, (SOCKADDR*)&addr, &sizofaddr); // Как только мы произвели подключение, все данные будут храниться тут

		if (nemConnection != 0) {
			cout << "Client connected\n";

			string path;
			cin >> path;
			send_file(&nemConnection, path);
		} 


		system("pause");
		return 0;
}