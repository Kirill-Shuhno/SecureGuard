#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include<sstream>
#include <iomanip>
#include <map>
#include <openssl/sha.h>


using namespace std;
namespace fs = std::filesystem;
const string database_file = "database.txt";

//записываем данные в базу данных
void save_database(const map<string, string>& database)
{
    ofstream db("database.txt");

    for (auto el : database)
    {
        db << el.first << "|" << el.second << endl; // el.first = ключ | el.second = значение
    }
}

// читаем данные из базы и получаем на выходе ключ:значение из уже созданной базы
map<string, string> load_database()
{
    map<string, string> hashes;

    ifstream db(database_file);

    string line;

    while (getline(db, line)) //берем 1 строку и ложим в line
    {
        stringstream ss(line); // создаем поток ss

        string path;
        string hash;

        getline(ss, path, '|');// читаем из ss путь до | и ложим в  path
        getline(ss, hash);// читаем(с последнего места тк ss запоминает позицию) остаток и ложим в  hash

        hashes[path] = hash;
    }

    return hashes;
}
//достаем хэш
string sha256(const string& str) {
    unsigned char hash[32];

    SHA256((unsigned char*)str.c_str(), str.size(), hash);// (unsigned char) надо потому что c_str() возвращает const char*
    //второй параметр сколько байт надо читать 

    stringstream ss; //  создаем строковый поток

    for (int i = 0; i < 32; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];//(int) нужен тк изначально байты хранятся символами а не числами
    }
    return ss.str();
}
// находим простой файл и запоминаем его путь и хэш
map<string, string> scan(const string& path_folder)
{
    map<string, string> hashes;

    for (const auto& e : fs::recursive_directory_iterator(path_folder))// e - текущий обьект | auto - сам определит тип обьекта
    {
        if (e.is_regular_file())// проверяет что обьект является простым файлом
        {
            string path = e.path().string();

            string hash = sha256(path);

            hashes[path] = hash;

            cout << "Scanned: " << path << endl;
        }
    }

    return hashes;
}
void check_file(
    const map<string, string>& old_hashes,
    const map<string, string>& new_hashes
)
{
    for (const auto& el : new_hashes)
    {
        auto a = old_hashes.find(el.first);//поиск по пути
        if (a == old_hashes.end())//end указывает на элемент после последнего элемента что говорит о том что путь не найден 
        {
            cout << "New: " << el.first << endl;
        }
        else if (a->second != el.second)
        {
            cout << "The file has been modified: " << el.first << endl;
        }

    }
    for (const auto& el : old_hashes)
    {
        if (new_hashes.find(el.first) == new_hashes.end())
        {
            cout << "The file " << el.first << " was deleted:" << endl;
        }
    }
}
//поиск сигнатуры
bool find_signature(ifstream& file)
{
    vector<unsigned char> data(
        (istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>()
    );

    vector<unsigned char> signature = {
        0x90, 0x90, 0xE8, 0xFF // сигнатура которую мы ищем
    };

    if (data.size() < signature.size())
    {
        cout << "file too small\n";
        return false;
    }
    bool find = false;
    for (size_t i = 0; i <= data.size() - signature.size(); i++)// обозначаем size_t тк  data.size() и signature.size() возвращают его
    {
        bool sign_find = true;
        for (size_t j = 0; j < signature.size(); j++) {
            if (data[i + j] != signature[j]) {
                sign_find = false;
                break;
            }
        }

        if (sign_find)
        {
            return true;//Сигнатура найдена!
        }
    }
    return false;//Сигнатура не найдена!
}

int main()
{
    string folder = "D:\\Unique\\FA\\couresework";
    if (!fs::exists(database_file))
    {
        cout << "Creating database\n";
        map<string, string> hash = scan(folder);
        save_database(hash);
        cout << "Database created.\n";
    }
    else
    {
        map<string, string> new_hash = scan(folder);
        map<string, string> old_hash = load_database();
        check_file(new_hash, old_hash);
        save_database(new_hash);//обновляем базу данных
    }



}