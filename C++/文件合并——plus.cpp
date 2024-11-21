#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <cctype>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>  // Windows ��������Ҫ��ͷ�ļ�
#define mkdir _mkdir   // Windows �ϵ� mkdir
#else
#include <sys/stat.h>  // Unix/Linux ��������Ҫ��ͷ�ļ�
#endif

// ��Ȼ����Ƚ���
bool natural_sort(const std::string &a, const std::string &b) {
    auto is_digit = [](char c) { return std::isdigit(c); };

    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        // �ҵ����ֲ���
        if (is_digit(a[i]) && is_digit(b[j])) {
            std::string num_a, num_b;
            while (i < a.size() && std::isdigit(a[i])) {
                num_a += a[i++];
            }
            while (j < b.size() && std::isdigit(b[j])) {
                num_b += b[j++];
            }
            // �Ƚ����ִ�С
            if (std::stoi(num_a) != std::stoi(num_b)) {
                return std::stoi(num_a) < std::stoi(num_b);
            }
        } else {
            if (a[i] != b[j]) {
                return a[i] < b[j];
            }
            ++i;
            ++j;
        }
    }
    return a.size() < b.size();
}

// �ж��ļ��Ƿ����ָ����׺
bool has_extension(const std::string& filename, const std::string& extension) {
    return filename.size() >= extension.size() &&
           filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
}

void copy_file(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dest(destination, std::ios::binary);

    if (!src.is_open() || !dest.is_open()) {
        std::cerr << "���ļ�ʧ�ܡ�" << std::endl;
        return;
    }

    dest << src.rdbuf();  // �����ļ�����
    std::cout << "�Ѹ���: " << source << " -> " << destination << std::endl;
}

void copy_and_rename_files(const std::string& source_dir, const std::string& target_dir, const std::string& name_prefix, int& counter, const std::string& extension) {
    DIR* dir = opendir(source_dir.c_str());
    if (dir == nullptr) {
        std::cerr << "�޷���Ŀ¼: " << source_dir << std::endl;
        return;
    }

    std::vector<std::string> files;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        
        // ���� "." �� ".."
        if (filename == "." || filename == "..") {
            continue;
        }

        // ֻ�������ָ����׺���ļ�
        if (has_extension(filename, extension)) {
            files.push_back(filename);  // �ռ��ļ���
        }
    }

    // ����Ȼ���������ļ�
    std::sort(files.begin(), files.end(), natural_sort);

    // �����ļ���������
    for (const auto& filename : files) {
        std::string old_path = source_dir + "/" + filename;
        std::string new_name = name_prefix + std::to_string(counter++) + "." + filename.substr(filename.find_last_of('.') + 1);
        std::string new_path = target_dir + "/" + new_name;

        copy_file(old_path, new_path);  // ���Ʋ��������ļ�
    }

    closedir(dir);  // �ر�Ŀ¼
}

int main() {
    std::string target_dir;
    int num_dirs;
    std::string extension;

    // ��ȡĿ���ļ���·��
    std::cout << "������Ŀ��Ŀ¼·��: ";
    std::cin >> target_dir;

    // ����Ŀ���ļ��У���������ڣ�
    #if defined(_WIN32) || defined(_WIN64)
        if (access(target_dir.c_str(), 0) == -1) {
            if (mkdir(target_dir.c_str()) == -1) {
                std::cerr << "����Ŀ��Ŀ¼ʧ�ܡ�" << std::endl;
                return 1;
            }
            std::cout << "�Ѵ���Ŀ��Ŀ¼: " << target_dir << std::endl;
        }
    #else
        if (access(target_dir.c_str(), F_OK) == -1) {
            if (mkdir(target_dir.c_str(), 0777) == -1) {
                std::cerr << "����Ŀ��Ŀ¼ʧ�ܡ�" << std::endl;
                return 1;
            }
            std::cout << "�Ѵ���Ŀ��Ŀ¼: " << target_dir << std::endl;
        }
    #endif

    // ��ȡԴ�ļ���·��
    std::cout << "������ԴĿ¼������: ";
    std::cin >> num_dirs;

    std::vector<std::string> source_dirs(num_dirs);

    std::cout << "������ԴĿ¼��·��: " << std::endl;
    for (int i = 0; i < num_dirs; i++) {
        std::cin >> source_dirs[i];
    }

    // ��ȡָ�����ļ���׺
    std::cout << "�������ļ���׺���磺.txt��.jpg�ȣ�: ";
    std::cin >> extension;

    std::string name_prefix = "file_";  // �ļ���ǰ׺
    int counter = 1;  // �ļ������ʼֵ

    // ����ԴĿ¼˳����д���
    for (int i = 0; i < num_dirs; i++) {
        copy_and_rename_files(source_dirs[i], target_dir, name_prefix, counter, extension);
    }

    std::cout << "�ļ��ϲ�����������ɣ�" << std::endl;
    return 0;
}

