import os
import sys

def generate_file_list():
    """生成所有文件的相对路径列表并保存到all_files.txt"""
    with open('all_files.txt', 'w', encoding='utf-8') as file_list:
        for root, dirs, files in os.walk('.'):
            # 跳过.git目录（可选）
            if '.git' in dirs:
                dirs.remove('.git')
                
            for filename in files:
                # 获取相对路径并标准化
                full_path = os.path.join(root, filename)
                rel_path = os.path.normpath(full_path)
                # 移除开头的./
                if rel_path.startswith('.\\') or rel_path.startswith('./'):
                    rel_path = rel_path[2:]
                file_list.write(rel_path + '\n')

def generate_directory_tree():
    """生成目录树并保存到directory_tree.txt"""
    with open('directory_tree.txt', 'w', encoding='utf-8') as tree_file:
        # 获取当前目录名称作为根节点
        base_name = os.path.basename(os.path.abspath('.'))
        tree_file.write(f"{base_name}/\n")
        
        # 递归生成目录树
        _generate_tree('.', '', tree_file)

def _generate_tree(current_dir, prefix, output_file):
    """递归生成目录树的辅助函数"""
    try:
        # 获取排序后的条目（目录在前，文件在后）
        entries = sorted(os.listdir(current_dir), 
                        key=lambda x: (not os.path.isdir(os.path.join(current_dir, x)), x))
        
        # 过滤掉.git目录（可选）
        entries = [e for e in entries if e != '.git']
        
        total = len(entries)
        for index, entry in enumerate(entries):
            is_last = (index == total - 1)
            full_path = os.path.join(current_dir, entry)
            
            # 确定当前条目的前缀符号
            if is_last:
                connector = '└── '
                next_prefix = prefix + '    '
            else:
                connector = '├── '
                next_prefix = prefix + '│   '
            
            if os.path.isdir(full_path):
                # 处理目录
                output_file.write(f"{prefix}{connector}{entry}/\n")
                # 递归处理子目录
                _generate_tree(full_path, next_prefix, output_file)
            else:
                # 处理文件
                output_file.write(f"{prefix}{connector}{entry}\n")
                
    except PermissionError:
        output_file.write(f"{prefix}{connector}[Permission Denied]\n")
    except Exception as e:
        output_file.write(f"{prefix}{connector}[Error: {str(e)}]\n")

if __name__ == "__main__":
    print("正在生成文件列表...")
    generate_file_list()
    
    print("正在生成目录树...")
    generate_directory_tree()
    
    print("操作完成！")
    print("文件列表已保存到: all_files.txt")
    print("目录树已保存到: directory_tree.txt")
