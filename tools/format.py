import os
import subprocess
import argparse


def format_files(starting_folder, file_extensions):
    """
    Recursively format files in the given folder and its subdirectories using clang-format.
    """
    for root, _, files in os.walk(starting_folder):
        for file in files:
            if file.endswith(tuple(file_extensions)):
                file_path = os.path.join(root, file)
                try:
                    print(f"Formatting: {file_path}")
                    subprocess.run(["clang-format", "-i", file_path], check=True)
                except subprocess.CalledProcessError as e:
                    print(f"Error formatting {file_path}: {e}")


def main():
    parser = argparse.ArgumentParser(description="Recursively run clang-format on a project.")
    parser.add_argument("starting_folder", type=str, help="Starting folder to recursively search for files.")
    parser.add_argument("--extensions", type=str, default=".cpp,.h,.hpp,.c",
                        help="Comma-separated list of file extensions to format. Default: .cpp,.h,.hpp,.c")
    args = parser.parse_args()

    file_extensions = args.extensions.split(",")
    if not os.path.exists(args.starting_folder):
        print(f"Error: Starting folder '{args.starting_folder}' does not exist.")
        return

    print(f"Starting clang-format in folder: {args.starting_folder}")
    format_files(args.starting_folder, file_extensions)
    print("Formatting complete!")


if __name__ == "__main__":
    main()
