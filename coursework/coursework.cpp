#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include<sstream>
#include <iomanip>
#include <openssl/sha.h>


using namespace std;
namespace fs = std::filesystem;

string sha256(const string& str) {
	unsigned char hash[32];

	SHA256((unsigned char*)str.c_str(),str.size(),hash);// (unsigned char*) надо потому что c_str() возвращает const char*
	//второй параметр сколько байт надо читать 

	stringstream ss; //  создаем строковый поток

	for (int i = 0; i < 32; i++)
	{
		ss << hex << setw(2) << setfill('0') << (int)hash[i];//(int) нужен тк изначально байты хранятся символами а не числами
	}
	return ss.str();
}

int main()
{
	ofstream file("text.txt", ios::binary);
	file << "Hello World";
	file.close();

	for (const auto& e : fs::directory_iterator("D:\\Unique\\FA\\coursework\\coursework")) {
		if (e.path().filename() == "text.txt")
		{
			cout << "File found" << endl;
			cout << e.path().extension() << endl;
		}
	}
	ifstream fileIn("text.txt", ios::binary);
	string data(
		(istreambuf_iterator<char>(fileIn)),
		istreambuf_iterator<char>()
	);
	string res = sha256(data);
	cout << res;
}
