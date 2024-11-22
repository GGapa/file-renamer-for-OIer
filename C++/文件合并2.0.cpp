
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#endif

bool is_directory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

std::string normalize_path(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    if (!result.empty() && result.back() == '/') {
        result.pop_back();
    }
    return result;
}

bool natural_sort(const std::string& a, const std::string& b) {
    auto is_digit = [](char c) { return std::isdigit(c); };

    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (is_digit(a[i]) && is_digit(b[j])) {
            std::string num_a, num_b;
            while (i < a.size() && is_digit(a[i])) num_a += a[i++];
            while (j < b.size() && is_digit(b[j])) num_b += b[j++];
            if (std::stoi(num_a) != std::stoi(num_b))
                return std::stoi(num_a) < std::stoi(num_b);
        } else {
            if (a[i] != b[j]) return a[i] < b[j];
            ++i; ++j;
        }
    }
    return a.size() < b.size();
}

void find_all_files(const std::string& base_dir, std::vector<std::string>& files) {
    DIR* dir = opendir(base_dir.c_str());
    if (dir == nullptr) {
        std::cerr << "无法打开目录: " << base_dir << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        if (name == "." || name == "..") continue;

        std::string full_path = base_dir + "/" + name;

        if (is_directory(full_path)) {
            find_all_files(full_path, files);
        } else {
            files.push_back(full_path);
        }
    }

    closedir(dir);
}

void copy_file(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dest(destination, std::ios::binary);

    if (!src.is_open() || !dest.is_open()) {
        std::cerr << "无法处理文件: " << source << std::endl;
        return;
    }

    dest << src.rdbuf();
    std::cout << "已复制: " << source << " -> " << destination << std::endl;
}

int main() {
    std::string root_dir, target_dir, custom_prefix;

    std::cout << "请输入根目录路径: ";
    std::cin >> root_dir;
    root_dir = normalize_path(root_dir);

    if (!is_directory(root_dir)) {
        std::cerr << "输入的根目录无效或不存在。" << std::endl;
        return 1;
    }

    std::cout << "请输入目标目录路径: ";
    std::cin >> target_dir;
    target_dir = normalize_path(target_dir);

    std::cout << "请输入文件名前缀: ";
    std::cin >> custom_prefix;

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

    std::vector<std::string> files;
    find_all_files(root_dir, files);

    if (files.empty()) {
        std::cout << "未找到任何文件！" << std::endl;
        return 1;
    }

    std::sort(files.begin(), files.end(), natural_sort);

    std::map<std::string, int> extension_counters;

    for (const auto& file_path : files) {
        std::string::size_type dot_pos = file_path.find_last_of('.');
        std::string extension = (dot_pos != std::string::npos) ? file_path.substr(dot_pos + 1) : "";

        int& counter = extension_counters[extension];
        ++counter;

        std::string new_name = custom_prefix + "_" + std::to_string(counter);
        if (!extension.empty()) {
            new_name += "." + extension;
        }

        std::string new_path = target_dir + "/" + new_name;

        copy_file(file_path, new_path);
    }

    std::cout << "所有文件已合并到目标目录并完成重命名！" << std::endl;
    return 0;
}
