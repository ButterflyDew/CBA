import sys

def read_first_line(file_path):
    try:
        with open(file_path, 'r') as file:
            first_line = file.readline().strip()
            return first_line
    except FileNotFoundError:
        print("File not found.")
        return None

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python read_first_line.py <file_path>")
        sys.exit(1)
    
    file_path = sys.argv[1]
    first_line = read_first_line(file_path)
    if first_line is not None:
        print("First line of the file:", first_line)
