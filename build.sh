#!/bin/bash

# 1. Check if FLTK is installed (Arch Linux specific)
# pacman -Qs returns 0 if found, 1 if not.
if ! pacman -Qs fltk > /dev/null; then
    echo "⚠️  FLTK library not found!"
    echo "📦 Installing fltk via pacman..."
    sudo pacman -S fltk --noconfirm
else
    echo "✅ FLTK library is already installed."
fi

# 2. Check if main.cpp exists
if [ ! -f "main.cpp" ]; then
    echo "❌ Error: main.cpp not found in the current directory."
    exit 1
fi

# 3. Compile the code
echo "🔨 Compiling main.cpp..."
g++ ehr_gui.cpp -lfltk -o ehr_gui

# 4. Run the application ONLY if compilation succeeded
# $? checks the exit code of the previous command (0 means success)
if [ $? -eq 0 ]; then
    echo "🚀 Compilation successful! Starting EHR GUI..."
    echo "----------------------------------------"
    ./ehr_gui
else
    echo "❌ Compilation failed. Please check your code errors above."
    exit 1
fi