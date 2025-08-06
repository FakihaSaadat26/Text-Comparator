# Makefile for Text Comparator Project
# Author: [Fakiha Saadat]
# Date: August 2025

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = text_comparator
SOURCE = main.cpp

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(SOURCE)
	@echo "🔨 Compiling Text Comparator..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)
	@echo "✅ Build successful! Run with: ./$(TARGET)"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(TARGET) result.txt
	@echo "✅ Clean complete!"

# Run the program
run: $(TARGET)
	@echo "🚀 Running Text Comparator..."
	./$(TARGET)

# Create sample test files
samples:
	@echo "📝 Creating sample text files..."
	@echo "This is a sample document. It contains multiple sentences. Some words may repeat in this document. The quick brown fox jumps over the lazy dog. This sentence demonstrates various word patterns and frequencies." > sample1.txt
	@echo "Another sample document for comparison. It has different content but may share some common words. The lazy dog sleeps under the tree. Testing word frequency and document analysis capabilities." > sample2.txt
	@echo "✅ Sample files created: sample1.txt, sample2.txt"

# Help target
help:
	@echo "📚 Text Comparator Makefile Help"
	@echo "================================"
	@echo "Available targets:"
	@echo "  all      - Build the text comparator (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run the program"
	@echo "  samples  - Create sample text files for testing"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make         # Build the program"
	@echo "  make run     # Build and run"
	@echo "  make samples # Create test files"
	@echo "  make clean   # Clean up"

.PHONY: all clean run samples help