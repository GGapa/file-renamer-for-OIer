import re
import time
import argparse

def escape_underscores_in_math(input_name, output_name):
    try:
        with open(input_name, 'r', encoding='utf8') as fr, open(output_name, 'w', encoding='utf8') as fw:
            data = fr.read()

            # 匹配数学公式，并仅替换公式中的下划线
            def replace_underscores(match):
                formula = match.group(0)
                # 忽略包裹符号（$ 或 $$），仅处理中间内容
                if formula.startswith('$$') and formula.endswith('$$'):
                    content = formula[2:-2]
                    return f"$$" + re.sub(r'_', r'\\_', content) + f"$$"
                elif formula.startswith('$') and formula.endswith('$'):
                    content = formula[1:-1]
                    return f"$" + re.sub(r'_', r'\\_', content) + f"$"
                return formula

            # 使用正则匹配 $...$ 或 $$...$$ 数学公式
            data = re.sub(r'\$\$(.+?)\$\$|\$(.+?)\$', replace_underscores, data, flags=re.DOTALL)
            fw.write(data)

        time.sleep(0.5)
        print('Success!')
    except FileNotFoundError as e:
        print(e)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Replace underscores with \\_ in math formulas only.")
    parser.add_argument('-i', '--input', required=True, help="Input file name")
    parser.add_argument('-o', '--output', required=True, help="Output file name")

    args = parser.parse_args()
    escape_underscores_in_math(args.input, args.output)

# 示例用法：
# python convert.py -i input.md -o output.md
