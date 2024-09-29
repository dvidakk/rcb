import shutil
import os
import sys

def copy_dll(src, dst):
    try:
        shutil.copy(src, dst)
        print(f"Copied {src} to {dst}")
    except Exception as e:
        print(f"Failed to copy {src} to {dst}: {e}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: copy_dll.py <src> <dst>")
        sys.exit(1)
    
    src = sys.argv[1]
    dst = sys.argv[2]
    
    copy_dll(src, dst)