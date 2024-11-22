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
#include <direct.h>  // Windows 环境下需要此头文件
#define mkdir _mkdir   // Windows 上的 mkdir
#else
#include <sys/stat.h>  // Unix/Linux 环境下需要此头文件
#endif

// 自然排序比较器
bool natural_sort(const std::string &a, const std::string &b) {
    auto is_digit = [](char c) { return std::isdigit(c); };

    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        // 找到数字部分
        if (is_digit(a[i]) && is_digit(b[j])) {
            std::string num_a, num_b;
            while (i < a.size() && std::isdigit(a[i])) {
                num_a += a[i++];
            }
            while (j < b.size() && std::isdigit(b[j])) {
                num_b += b[j++];
            }
            // 比较数字大小
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

// 判断文件是否具有指定后缀
bool has_extension(const std::string& filename, const std::string& extension) {
    return filename.size() >= extension.size() &&
           filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0;
}

void copy_file(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dest(destination, std::ios::binary);

    if (!src.is_open() || !dest.is_open()) {
        std::cerr << "打开文件失败。" << std::endl;
        return;
    }

    dest << src.rdbuf();  // 复制文件内容
    std::cout << "已复制: " << source << " -> " << destination << std::endl;
}

void copy_and_rename_files(const std::string& source_dir, const std::string& target_dir, const std::string& name_prefix, int& counter, const std::string& extension) {
    DIR* dir = opendir(source_dir.c_str());
    if (dir == nullptr) {
        std::cerr << "无法打开目录: " << source_dir << std::endl;
        return;
    }

    std::vector<std::string> files;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        
        // 跳过 "." 和 ".."
        if (filename == "." || filename == "..") {
            continue;
        }

        // 只处理具有指定后缀的文件
        if (has_extension(filename, extension)) {
            files.push_back(filename);  // 收集文件名
        }
    }

    // 按自然排序排序文件
    std::sort(files.begin(), files.end(), natural_sort);

    // 复制文件并重命名
    for (const auto& filename : files) {
        std::string old_path = source_dir + "/" + filename;
        std::string new_name = name_prefix + std::to_string(counter++) + "." + filename.substr(filename.find_last_of('.') + 1);
        std::string new_path = target_dir + "/" + new_name;

        copy_file(old_path, new_path);  // 复制并重命名文件
    }

    closedir(dir);  // 关闭目录
}

int main() {
    std::string target_dir;
    int num_dirs;
    std::string extension;

    // 读取目标文件夹路径
    std::cout << "请输入目标目录路径: ";
    std::cin >> target_dir;

    // 创建目标文件夹（如果不存在）
    #if defined(_WIN32) || defined(_WIN64)
        if (access(target_dir.c_str(), 0) == -1) {
            if (mkdir(target_dir.c_str()) == -1) {
                std::cerr << "创建目标目录失败。" << std::endl;
                return 1;
            }
            std::cout << "已创建目标目录: " << target_dir << std::endl;
        }
    #else
        if (access(target_dir.c_str(), F_OK) == -1) {
            if (mkdir(target_dir.c_str(), 0777) == -1) {
                std::cerr << "创建目标目录失败。" << std::endl;
                return 1;
            }
            std::cout << "已创建目标目录: " << target_dir << std::endl;
        }
    #endif

    // 读取源文件夹路径
    std::cout << "请输入源目录的数量: ";
    std::cin >> num_dirs;

    std::vector<std::string> source_dirs(num_dirs);

    std::cout << "请输入源目录的路径: " << std::endl;
    for (int i = 0; i < num_dirs; i++) {
        std::cin >> source_dirs[i];
    }

    // 读取指定的文件后缀
    std::cout << "请输入文件后缀（如：.txt、.jpg等）: ";
    std::cin >> extension;

    std::string name_prefix = "file_";  // 文件名前缀
    int counter = 1;  // 文件编号起始值

    // 按照源目录顺序进行处理
    for (int i = 0; i < num_dirs; i++) {
        copy_and_rename_files(source_dirs[i], target_dir, name_prefix, counter, extension);
    }

    std::cout << "文件合并和重命名完成！" << std::endl;
    return 0;
}

