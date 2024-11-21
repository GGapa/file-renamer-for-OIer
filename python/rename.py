import os
import re
import shutil
from collections import defaultdict

def natural_sort_key(s):
    """自然排序键"""
    return [int(text) if text.isdigit() else text.lower() for text in re.split(r'(\d+)', s)]

def rename_files_in_directory(directory, prefix):
    """处理当前目录中的文件重命名"""
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    files.sort(key=natural_sort_key)
    
    suffix_counters = defaultdict(int)  # 每种后缀的计数
    for file in files:
        full_path = os.path.join(directory, file)
        name, ext = os.path.splitext(file)
        ext = ext.lower()  # 统一后缀格式
        
        suffix_counters[ext] += 1
        new_name = f"{prefix}{suffix_counters[ext]}{ext}"
        os.rename(full_path, os.path.join(directory, new_name))

def process_subfolders(root_dir, prefix):
    """处理子文件夹的文件重命名"""
    folders = [f for f in os.listdir(root_dir) if os.path.isdir(os.path.join(root_dir, f))]
    folders.sort(key=natural_sort_key)
    
    suffix_counters = defaultdict(int)  # 全局后缀计数
    for folder in folders:
        folder_path = os.path.join(root_dir, folder)
        files = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]
        files.sort(key=natural_sort_key)
        
        for file in files:
            full_path = os.path.join(folder_path, file)
            name, ext = os.path.splitext(file)
            ext = ext.lower()
            
            suffix_counters[ext] += 1
            new_name = f"{prefix}{suffix_counters[ext]}{ext}"
            shutil.move(full_path, os.path.join(root_dir, new_name))  # 移动到根目录
        
        os.rmdir(folder_path)  # 删除空文件夹

def rename_tool(root_dir, prefix):
    """文件重命名工具主函数"""
    if not os.path.exists(root_dir):
        print("指定目录不存在！")
        return
    
    if all(os.path.isfile(os.path.join(root_dir, f)) for f in os.listdir(root_dir)):
        # 当前目录下没有子文件夹
        rename_files_in_directory(root_dir, prefix)
    else:
        # 当前目录下包含子文件夹
        process_subfolders(root_dir, prefix)

if __name__ == "__main__":
    # 获取用户输入的路径和前缀
    root_directory = input("请输入根目录路径：").strip()
    file_prefix = input("请输入文件前缀：").strip()
    rename_tool(root_directory, file_prefix)
