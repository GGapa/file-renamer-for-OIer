import os
import re
import shutil
from collections import defaultdict
from datetime import datetime

def natural_sort_key(s):
    """自然排序键"""
    return [int(text) if text.isdigit() else text.lower() for text in re.split(r'(\d+)', s)]

def write_log(log_file, message):
    """将日志写入文件"""
    with open(log_file, "a", encoding="utf-8") as log:
        log.write(message + "\n")

def rename_files_in_directory(directory, prefix, log_file):
    """处理当前目录中的文件重命名"""
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    files.sort(key=natural_sort_key)
    
    suffix_counters = defaultdict(int)  # 每种后缀的计数
    for file in files:
        if file == "rename_log.txt":  # 跳过日志文件
            continue
        
        full_path = os.path.join(directory, file)
        name, ext = os.path.splitext(file)
        ext = ext.lower()  # 统一后缀格式
        
        suffix_counters[ext] += 1
        new_name = f"{prefix}{suffix_counters[ext]}{ext}"
        new_path = os.path.join(directory, new_name)
        os.rename(full_path, new_path)
        
        log_message = f"[RENAME] {full_path} -> {new_path}"
        print(log_message)
        write_log(log_file, log_message)

def process_subfolders(root_dir, prefix, log_file):
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
            new_path = os.path.join(root_dir, new_name)
            shutil.move(full_path, new_path)  # 移动到根目录
            
            log_message = f"[MOVE & RENAME] {full_path} -> {new_path}"
            print(log_message)
            write_log(log_file, log_message)
        
        os.rmdir(folder_path)  # 删除空文件夹
        log_message = f"[REMOVE FOLDER] {folder_path}"
        print(log_message)
        write_log(log_file, log_message)

def rename_tool(root_dir, prefix):
    """文件重命名工具主函数"""
    if not os.path.exists(root_dir):
        print("指定目录不存在！")
        return
    
    # 创建日志文件
    log_file = os.path.join(root_dir, "rename_log.txt")
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    write_log(log_file, f"[START] 文件重命名工具开始运行 - {timestamp}")
    
    if all(os.path.isfile(os.path.join(root_dir, f)) for f in os.listdir(root_dir) if f != "rename_log.txt"):
        # 当前目录下没有子文件夹
        write_log(log_file, "[INFO] 当前目录无子文件夹，处理文件...")
        rename_files_in_directory(root_dir, prefix, log_file)
    else:
        # 当前目录下包含子文件夹
        write_log(log_file, "[INFO] 当前目录包含子文件夹，逐一处理文件夹...")
        process_subfolders(root_dir, prefix, log_file)
    
    write_log(log_file, "[COMPLETE] 文件重命名工具运行结束")

if __name__ == "__main__":
    # 获取用户输入的路径和前缀
    root_directory = input("请输入根目录路径：").strip()
    file_prefix = input("请输入文件前缀：").strip()
    rename_tool(root_directory, file_prefix)
