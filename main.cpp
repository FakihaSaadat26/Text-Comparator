#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cmath>

using namespace std;

// Structure to hold document statistics
struct DocumentStats {
    string filename;
    int wordCount;
    int sentenceCount;
    double avgSentenceLength;
    string longestSentence;
    int longestSentenceWordCount;
    set<string> uniqueWords;
    map<string, int> wordFrequency;
    vector<pair<string, int>> topWords;
};

// Function declarations
string loadFile(const string& filename);
vector<string> tokenize(const string& text);
string normalizeText(const string& text);
int countWords(const vector<string>& tokens);
int countSentences(const string& text);
double calculateAvgSentenceLength(int wordCount, int sentenceCount);
vector<pair<string, int>> getTopFrequentWords(const map<string, int>& wordFreq, int n = 5);
set<string> getUniqueWords(const vector<string>& tokens);
map<string, int> getWordFrequency(const vector<string>& tokens);
set<string> findCommonWords(const set<string>& words1, const set<string>& words2);
pair<string, int> findLongestSentence(const string& text);
double jaccardSimilarity(const set<string>& set1, const set<string>& set2);
map<string, pair<int, int>> getCommonWordsWithCounts(const DocumentStats& doc1, const DocumentStats& doc2);
void printCommonWordsAnalysis(const DocumentStats& doc1, const DocumentStats& doc2);
void printHeader();
void printSeparator(char ch = '=', int length = 80);
void printComparisonTable(const DocumentStats& doc1, const DocumentStats& doc2, double similarity);
void writeReportToFile(const DocumentStats& doc1, const DocumentStats& doc2, double similarity, const set<string>& commonWords);
void performWordReplacement(const string& originalFile1, const string& originalFile2);
string replaceWordInText(const string& text, const string& oldWord, const string& newWord);
int countWordOccurrences(const string& text, const string& word);
DocumentStats analyzeDocument(const string& filename);
void generateUpdatedReport(const string& file1, const string& file2, const string& oldWord, const string& newWord);

int main() {
    printHeader();
    
    string file1, file2;
    
    cout << "\n Enter path to first document: ";
    getline(cin, file1);
    
    cout << " Enter path to second document: ";
    getline(cin, file2);
    
    cout << "\n Processing documents: \n";
    printSeparator('-', 50);
    
    // Analyze both documents
    DocumentStats doc1 = analyzeDocument(file1);
    DocumentStats doc2 = analyzeDocument(file2);
    
    if (doc1.wordCount == 0 || doc2.wordCount == 0) {
        cout << " Error: Could not process one or both documents.\n";
        return 1;
    }
    
    // Calculate similarity
    double similarity = jaccardSimilarity(doc1.uniqueWords, doc2.uniqueWords);
    
    // Display results
    printComparisonTable(doc1, doc2, similarity);
    
    // Common words analysis
    printCommonWordsAnalysis(doc1, doc2);
    
    // Find common words
    set<string> commonWords = findCommonWords(doc1.uniqueWords, doc2.uniqueWords);
    
    // Write detailed report
    writeReportToFile(doc1, doc2, similarity, commonWords);
    
    cout << "\n Completed Successfully!\n Detailed report saved to 'result.txt'\n";
    printSeparator();
    
    // Ask for word replacement
    performWordReplacement(file1, file2);
    
    return 0;
}

string loadFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << " Error: Cannot open file '" << filename << "'\n";
        return "";
    }
    
    string content, line;
    while (getline(file, line)) {
        content += line + " ";
    }
    file.close();
    
    cout << " Successfully loaded into the " << filename << "\n";
    return content;
}

string normalizeText(const string& text) {
    string normalized;
    for (char c : text) {
        if (isalpha(c) || isspace(c) || c == '.' || c == '!' || c == '?') {
            normalized += tolower(c);
        } else if (ispunct(c)) {
            normalized += ' ';
        }
    }
    return normalized;
}

vector<string> tokenize(const string& text) {
    vector<string> tokens;
    istringstream iss(normalizeText(text));
    string word;
    
    while (iss >> word) {
        if (!word.empty() && isalpha(word[0])) {
            tokens.push_back(word);
        }
    }
    return tokens;
}

int countWords(const vector<string>& tokens) {
    return tokens.size();
}

int countSentences(const string& text) {
    int count = 0;
    for (char c : text) {
        if (c == '.' || c == '!' || c == '?') {
            count++;
        }
    }
    return count > 0 ? count : 1; // At least 1 sentence
}

double calculateAvgSentenceLength(int wordCount, int sentenceCount) {
    return sentenceCount > 0 ? static_cast<double>(wordCount) / sentenceCount : 0.0;
}

map<string, int> getWordFrequency(const vector<string>& tokens) {
    map<string, int> frequency;
    for (const string& word : tokens) {
        frequency[word]++;
    }
    return frequency;
}

vector<pair<string, int>> getTopFrequentWords(const map<string, int>& wordFreq, int n) {
    vector<pair<string, int>> words(wordFreq.begin(), wordFreq.end());
    
    sort(words.begin(), words.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });
    
    if (words.size() > n) {
        words.resize(n);
    }
    
    return words;
}

set<string> getUniqueWords(const vector<string>& tokens) {
    return set<string>(tokens.begin(), tokens.end());
}

set<string> findCommonWords(const set<string>& words1, const set<string>& words2) {
    set<string> common;
    set_intersection(words1.begin(), words1.end(),
                    words2.begin(), words2.end(),
                    inserter(common, common.begin()));
    return common;
}
pair<string, int> findLongestSentence(const string& text) {
    string longestSentence;
    int maxWords = 0;
    
    istringstream iss(text);
    string sentence;
    char c;
    
    while (iss.get(c)) {
        sentence += c;
        if (c == '.' || c == '!' || c == '?') {
            vector<string> words = tokenize(sentence);
            if (words.size() > maxWords) {
                maxWords = words.size();
                longestSentence = sentence;
            }
            sentence.clear();
        }
    }
    
    // Handle case where text doesn't end with punctuation
    if (!sentence.empty()) {
        vector<string> words = tokenize(sentence);
        if (words.size() > maxWords) {
            maxWords = words.size();
            longestSentence = sentence;
        }
    }
    
    return make_pair(longestSentence, maxWords);
}

double jaccardSimilarity(const set<string>& set1, const set<string>& set2) {
    set<string> intersection;
    set<string> unionSet;
    
    set_intersection(set1.begin(), set1.end(),
                    set2.begin(), set2.end(),
                    inserter(intersection, intersection.begin()));
    
    set_union(set1.begin(), set1.end(),
             set2.begin(), set2.end(),
             inserter(unionSet, unionSet.begin()));
    
    if (unionSet.empty()) return 0.0;
    
    return static_cast<double>(intersection.size()) / unionSet.size() * 100.0;
}

DocumentStats analyzeDocument(const string& filename) {
    DocumentStats stats;
    stats.filename = filename;
    
    string content = loadFile(filename);
    if (content.empty()) {
        stats.wordCount = 0;
        return stats;
    }
    
    vector<string> tokens = tokenize(content);
    
    stats.wordCount = countWords(tokens);
    stats.sentenceCount = countSentences(content);
    stats.avgSentenceLength = calculateAvgSentenceLength(stats.wordCount, stats.sentenceCount);
    stats.uniqueWords = getUniqueWords(tokens);
    stats.wordFrequency = getWordFrequency(tokens);
    stats.topWords = getTopFrequentWords(stats.wordFrequency);
    
    auto longestInfo = findLongestSentence(content);
    stats.longestSentence = longestInfo.first;
    stats.longestSentenceWordCount = longestInfo.second;
    
    return stats;
}

void printHeader() {
    printSeparator();
    cout << setw(40) << " TEXT COMPARATOR \n";
    cout << setw(42) << "Document Analysis & Comparison Tool\n";
    printSeparator();
}

void printSeparator(char ch, int length) {
    cout << string(length, ch) << "\n";
}

void printComparisonTable(const DocumentStats& doc1, const DocumentStats& doc2, double similarity) {
    cout << "\n COMPARISON RESULTS\n";
    printSeparator('-', 80);
    
    cout << left << setw(25) << "Metric" 
         << setw(25) << "Document A" 
         << setw(25) << "Document B" << "\n";
    printSeparator('-', 75);
    
    cout << left << setw(25) << " Filename:" 
         << setw(25) << doc1.filename.substr(0, 22) 
         << setw(25) << doc2.filename.substr(0, 22) << "\n";
    
    cout << left << setw(25) << " Word Count:" 
         << setw(25) << doc1.wordCount 
         << setw(25) << doc2.wordCount << "\n";
    
    cout << left << setw(25) << " Sentence Count:" 
         << setw(25) << doc1.sentenceCount 
         << setw(25) << doc2.sentenceCount << "\n";
    
    cout << left << setw(25) << "Unique Words:" 
         << setw(25) << doc1.uniqueWords.size() 
         << setw(25) << doc2.uniqueWords.size() << "\n";
    
    cout << left << setw(25) << " Avg Sentence Length:" 
         << setw(25) << fixed << setprecision(2) << doc1.avgSentenceLength 
         << setw(25) << doc2.avgSentenceLength << "\n";
    
    cout << left << setw(25) << " Longest Sentence:" 
         << setw(25) << doc1.longestSentenceWordCount 
         << setw(25) << doc2.longestSentenceWordCount << "\n";
    
    printSeparator('-', 75);
    
    cout << "\n SIMILARITY ANALYSIS\n";
    printSeparator('-', 30);
    cout << " Jaccard Similarity: " << fixed << setprecision(2) << similarity << "%\n";
    
    set<string> commonWords = findCommonWords(doc1.uniqueWords, doc2.uniqueWords);
    cout << " Common Words: " << commonWords.size() << "\n";
    
    cout << "\n TOP 5 FREQUENT WORDS\n";
    printSeparator('-', 50);
    
    cout << left << setw(15) << "Document A" << setw(15) << "Document B" << "\n";
    printSeparator('-', 30);
    
    for (int i = 0; i < 5; i++) {
        string word1 = (i < doc1.topWords.size()) ? 
                       doc1.topWords[i].first + "(" + to_string(doc1.topWords[i].second) + ")" : "-";
        string word2 = (i < doc2.topWords.size()) ? 
                       doc2.topWords[i].first + "(" + to_string(doc2.topWords[i].second) + ")" : "-";
        
        cout << left << setw(15) << word1 << setw(15) << word2 << "\n";
    }
}

void writeReportToFile(const DocumentStats& doc1, const DocumentStats& doc2, double similarity, const set<string>& commonWords) {
    ofstream report("result.txt");
    if (!report.is_open()) {
        cout << " Error: Cannot create result.txt\n";
        return;
    }
    
    report << "TEXT COMPARATOR - DETAILED ANALYSIS REPORT\n";
    report << "==========================================\n\n";
    
    report << "DOCUMENT ANALYSIS\n";
    report << "-----------------\n\n";
    
    // Document A Analysis
    report << "Document A: " << doc1.filename << "\n";
    report << "- Word Count: " << doc1.wordCount << "\n";
    report << "- Sentence Count: " << doc1.sentenceCount << "\n";
    report << "- Unique Words: " << doc1.uniqueWords.size() << "\n";
    report << "- Average Sentence Length: " << fixed << setprecision(2) << doc1.avgSentenceLength << " words\n";
    report << "- Longest Sentence: " << doc1.longestSentenceWordCount << " words\n";
    report << "- Top 5 Words: ";
    for (int i = 0; i < min(5, (int)doc1.topWords.size()); i++) {
        report << doc1.topWords[i].first << "(" << doc1.topWords[i].second << ")";
        if (i < min(4, (int)doc1.topWords.size() - 1)) report << ", ";
    }
    report << "\n\n";
    
    // Document B Analysis
    report << "Document B: " << doc2.filename << "\n";
    report << "- Word Count: " << doc2.wordCount << "\n";
    report << "- Sentence Count: " << doc2.sentenceCount << "\n";
    report << "- Unique Words: " << doc2.uniqueWords.size() << "\n";
    report << "- Average Sentence Length: " << fixed << setprecision(2) << doc2.avgSentenceLength << " words\n";
    report << "- Longest Sentence: " << doc2.longestSentenceWordCount << " words\n";
    report << "- Top 5 Words: ";
    for (int i = 0; i < min(5, (int)doc2.topWords.size()); i++) {
        report << doc2.topWords[i].first << "(" << doc2.topWords[i].second << ")";
        if (i < min(4, (int)doc2.topWords.size() - 1)) report << ", ";
    }
    report << "\n\n";
    
    // Comparison Analysis
    report << "COMPARISON ANALYSIS\n";
    report << "-------------------\n\n";
    report << "Jaccard Similarity: " << fixed << setprecision(2) << similarity << "%\n";
    report << "Common Words Count: " << commonWords.size() << "\n\n";
    
    report << "Common Words: ";
    int count = 0;
    for (const string& word : commonWords) {
        if (count > 0) report << ", ";
        report << word;
        count++;
        if (count >= 20) {
            report << "... (and " << (commonWords.size() - 20) << " more)";
            break;
        }
    }
    report << "\n\n";
     
    report << "LONGEST SENTENCES\n";
    report << "-----------------\n\n";
    report << "Document A longest sentence (" << doc1.longestSentenceWordCount << " words):\n";
    report << doc1.longestSentence << "\n\n";
    report << "Document B longest sentence (" << doc2.longestSentenceWordCount << " words):\n";
    report << doc2.longestSentence << "\n\n";
    
    report << "End of Report\n";
    report.close();
}

// WORD REPLACEMENT FUNCTIONALITY

void performWordReplacement(const string& originalFile1, const string& originalFile2) {
    char choice;
    cout << "\n WORD REPLACEMENT FEATURE\n";
    printSeparator('-', 40);
    cout << "Would you like to replace any word in the documents? (y/n): ";
    cin >> choice;
    cin.ignore(); // Clear the input buffer
    
    if (choice == 'y' || choice == 'Y') {
        cout << "\n REPLACEMENT OPTIONS:\n";
        cout << "1. Replace word in both documents\n";
        cout << "2. Replace word in first document only (" << originalFile1 << ")\n";
        cout << "3. Replace word in second document only (" << originalFile2 << ")\n";
        cout << "\n Select option (1/2/3): ";
        
        int option;
        cin >> option;
        cin.ignore();
        
        string oldWord, newWord;
        cout << "\n Enter the word you want to replace: ";
        getline(cin, oldWord);
        cout << " Enter the replacement word: ";
        getline(cin, newWord);
        
        vector<string> filesToProcess;
        vector<string> updatedFiles;
        bool filesUpdated = false;
        
        // Determine which files to process
        switch(option) {
            case 1:
                filesToProcess.push_back(originalFile1);
                filesToProcess.push_back(originalFile2);
                break;
            case 2:
                filesToProcess.push_back(originalFile1);
                break;
            case 3:
                filesToProcess.push_back(originalFile2);
                break;
            default:
                cout << " Invalid option selected.\n";
                return;
        }
        
        cout << "\n Processing files: \n";
        printSeparator('-', 30);
        
        // Process each selected file
        for (const string& filename : filesToProcess) {
            cout << "\n Processing: " << filename << "\n";
            
            // Load the file content
            string content = loadFile(filename);
            if (content.empty()) {
                cout << "Error: Could not load " << filename << " for replacement.\n";
                continue;
            }
            
            // Count occurrences before replacement
            int occurrences = countWordOccurrences(content, oldWord);
            if (occurrences == 0) {
                cout << " Word '" << oldWord << "' not found in " << filename << ".\n";
                continue;
            }
            
            cout << " Found " << occurrences << " occurrence(s) of '" << oldWord << "'\n";
            
            // Perform replacement
            string updatedContent = replaceWordInText(content, oldWord, newWord);
            
            // Create new filename
            string newFilename;
            size_t dotPos = filename.find_last_of('.');
            if (dotPos != string::npos) {
                newFilename = filename.substr(0, dotPos) + "_updated" + filename.substr(dotPos);
            } else {
                newFilename = filename + "_updated.txt";
            }
            
            // Write updated content to new file
            ofstream outputFile(newFilename);
            if (!outputFile.is_open()) {
                cout << " Error: Could not create output file " << newFilename << ".\n";
                continue;
            }
            
            outputFile << updatedContent;
            outputFile.close();
            
            cout << " SUCCESS!\n";
            cout << " Original file: " << filename << "\n";
            cout << " Updated file: " << newFilename << "\n";
            cout << " Replaced " << occurrences << " occurrence(s) of '" << oldWord << "' with '" << newWord << "'\n";
            
            updatedFiles.push_back(newFilename);
            filesUpdated = true;
        }
        
        // Generate new report if files were updated
        if (filesUpdated && !updatedFiles.empty()) {
            cout << "\n GENERATING UPDATED ANALYSIS REPORT...\n";
            printSeparator('-', 40);
            
            string file1ForReport, file2ForReport;
            
            if (option == 1 && updatedFiles.size() == 2) {
                // Both files updated
                file1ForReport = updatedFiles[0];
                file2ForReport = updatedFiles[1];
            } else if (option == 2) {
                // Only first file updated
                file1ForReport = updatedFiles[0];
                file2ForReport = originalFile2;
            } else if (option == 3) {
                // Only second file updated
                file1ForReport = originalFile1;
                file2ForReport = updatedFiles[0];
            }
            
            generateUpdatedReport(file1ForReport, file2ForReport, oldWord, newWord);
        }
        
        // Ask if user wants to replace more words
        cout << "\nWould you like to replace another word? (y/n): ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 'y' || choice == 'Y') {
            performWordReplacement(originalFile1, originalFile2);
        }
    } else {
        cout << " No word replacement requested.\n";
    }
}

string replaceWordInText(const string& text, const string& oldWord, const string& newWord) {
    string result = text;
    string oldWordLower = oldWord;
    transform(oldWordLower.begin(), oldWordLower.end(), oldWordLower.begin(), ::tolower);
    
    // Convert to lowercase for case-insensitive matching
    string textLower = text;
    transform(textLower.begin(), textLower.end(), textLower.begin(), ::tolower);
    
    size_t pos = 0;
    while ((pos = textLower.find(oldWordLower, pos)) != string::npos) {
        // Check if it's a whole word (not part of another word)
        bool isWholeWord = true;
        
        // Check character before
        if (pos > 0 && isalnum(textLower[pos - 1])) {
            isWholeWord = false;
        }
        
        // Check character after
        if (pos + oldWordLower.length() < textLower.length() && 
            isalnum(textLower[pos + oldWordLower.length()])) {
            isWholeWord = false;
        }
        
        if (isWholeWord) {
            // Replace in the original text (preserving original case)
            result.replace(pos, oldWord.length(), newWord);
            textLower.replace(pos, oldWordLower.length(), newWord);
            pos += newWord.length();
        } else {
            pos += oldWordLower.length();
        }
    }
    
    return result;
}

int countWordOccurrences(const string& text, const string& word) {
    int count = 0;
    string wordLower = word;
    transform(wordLower.begin(), wordLower.end(), wordLower.begin(), ::tolower);
    
    string textLower = text;
    transform(textLower.begin(), textLower.end(), textLower.begin(), ::tolower);
    
    size_t pos = 0;
    while ((pos = textLower.find(wordLower, pos)) != string::npos) {
        // Check if it's a whole word
        bool isWholeWord = true;
        
        if (pos > 0 && isalnum(textLower[pos - 1])) {
            isWholeWord = false;
        }
        
        if (pos + wordLower.length() < textLower.length() && 
            isalnum(textLower[pos + wordLower.length()])) {
            isWholeWord = false;
        }
        
        if (isWholeWord) {
            count++;
        }
        pos += wordLower.length();
    }
    
    return count;
}
map<string, pair<int, int>> getCommonWordsWithCounts(const DocumentStats& doc1, const DocumentStats& doc2) {
    map<string, pair<int, int>> commonWords;
    
    for (const auto& word1 : doc1.wordFrequency) {
        auto it = doc2.wordFrequency.find(word1.first);
        if (it != doc2.wordFrequency.end()) {
            commonWords[word1.first] = make_pair(word1.second, it->second);
        }
    }
    
    return commonWords;
}
void printCommonWordsAnalysis(const DocumentStats& doc1, const DocumentStats& doc2) {
    cout << "\n COMMON WORDS DETAILED ANALYSIS\n";
    printSeparator('=', 70);
    
    auto commonWords = getCommonWordsWithCounts(doc1, doc2);
    
    if (commonWords.empty()) {
        cout << " No common words found between the documents.\n";
        return;
    }
    // Sort by total frequency (sum of both documents)
    vector<pair<string, pair<int, int>>> sortedCommon(commonWords.begin(), commonWords.end());
    sort(sortedCommon.begin(), sortedCommon.end(), 
         [](const auto& a, const auto& b) {
             return (a.second.first + a.second.second) > (b.second.first + b.second.second);
         });
    
    cout << left << setw(15) << "Word" 
         << setw(12) << "Doc A Count" 
         << setw(12) << "Doc B Count"
         << setw(15) << "Total Uses"
         << setw(15) << "Frequency %" << "\n";
    printSeparator('-', 70);
    
    int totalWordsDoc1 = doc1.wordCount;
    int totalWordsDoc2 = doc2.wordCount;
    
    for (size_t i = 0; i < min(size_t(15), sortedCommon.size()); i++) {
        const string& word = sortedCommon[i].first;
        int count1 = sortedCommon[i].second.first;
        int count2 = sortedCommon[i].second.second;
        int totalUses = count1 + count2;
        
        double freq1 = (static_cast<double>(count1) / totalWordsDoc1) * 100;
        double freq2 = (static_cast<double>(count2) / totalWordsDoc2) * 100;
        double avgFreq = (freq1 + freq2) / 2;
        
        cout << left << setw(15) << word
             << setw(12) << count1
             << setw(12) << count2
             << setw(15) << totalUses
             << setw(15) << (to_string(avgFreq).substr(0, 4) + "%") << "\n";
    }
}

void generateUpdatedReport(const string& file1, const string& file2, const string& oldWord, const string& newWord) {
    cout << " Analyzing updated documents...\n";
    
    // Analyze both documents
    DocumentStats doc1 = analyzeDocument(file1);
    DocumentStats doc2 = analyzeDocument(file2);
    
    if (doc1.wordCount == 0 || doc2.wordCount == 0) {
        cout << " Error: Could not process one or both updated documents.\n";
        return;
    }
    
    // Calculate similarity
    double similarity = jaccardSimilarity(doc1.uniqueWords, doc2.uniqueWords);
    
    // Find common words
    set<string> commonWords = findCommonWords(doc1.uniqueWords, doc2.uniqueWords);
    
    // Create updated report filename
    string reportFilename = "result_updated.txt";
    
    ofstream report(reportFilename);
    if (!report.is_open()) {
        cout << " Error: Cannot create " << reportFilename << "\n";
        return;
    }
    
    report << "TEXT COMPARATOR - UPDATED ANALYSIS REPORT\n";
    report << "==========================================\n\n";
    
    report << "WORD REPLACEMENT SUMMARY\n";
    report << "------------------------\n";
    report << "Replaced word: '" << oldWord << "' -> '" << newWord << "'\n";
    report << "Documents analyzed: " << file1 << " and " << file2 << "\n\n";
    
    report << "DOCUMENT ANALYSIS (AFTER REPLACEMENT)\n";
    report << "------------------------------------\n\n";
    
    // Document A Analysis
    report << "Document A: " << doc1.filename << "\n";
    report << "- Word Count: " << doc1.wordCount << "\n";
    report << "- Sentence Count: " << doc1.sentenceCount << "\n";
    report << "- Unique Words: " << doc1.uniqueWords.size() << "\n";
    report << "- Average Sentence Length: " << fixed << setprecision(2) << doc1.avgSentenceLength << " words\n";
    report << "- Longest Sentence: " << doc1.longestSentenceWordCount << " words\n";
    report << "- Top 5 Words: ";
    for (int i = 0; i < min(5, (int)doc1.topWords.size()); i++) {
        report << doc1.topWords[i].first << "(" << doc1.topWords[i].second << ")";
        if (i < min(4, (int)doc1.topWords.size() - 1)) report << ", ";
    }
    report << "\n\n";
    
    // Document B Analysis
    report << "Document B: " << doc2.filename << "\n";
    report << "- Word Count: " << doc2.wordCount << "\n";
    report << "- Sentence Count: " << doc2.sentenceCount << "\n";
    report << "- Unique Words: " << doc2.uniqueWords.size() << "\n";
    report << "- Average Sentence Length: " << fixed << setprecision(2) << doc2.avgSentenceLength << " words\n";
    report << "- Longest Sentence: " << doc2.longestSentenceWordCount << " words\n";
    report << "- Top 5 Words: ";
    for (int i = 0; i < min(5, (int)doc2.topWords.size()); i++) {
        report << doc2.topWords[i].first << "(" << doc2.topWords[i].second << ")";
        if (i < min(4, (int)doc2.topWords.size() - 1)) report << ", ";
    }
    report << "\n\n";
    
    // Comparison Analysis
    report << "COMPARISON ANALYSIS (AFTER REPLACEMENT)\n";
    report << "---------------------------------------\n\n";
    report << "Jaccard Similarity: " << fixed << setprecision(2) << similarity << "%\n";
    report << "Common Words Count: " << commonWords.size() << "\n\n";
    
    report << "Common Words: ";
    int count = 0;
    for (const string& word : commonWords) {
        if (count > 0) report << ", ";
        report << word;
        count++;
        if (count >= 20) {
            report << "... (and " << (commonWords.size() - 20) << " more)";
            break;
        }
    }
    report << "\n\n";
    
    report << "LONGEST SENTENCES (AFTER REPLACEMENT)\n";
    report << "------------------------------------\n\n";
    report << "Document A longest sentence (" << doc1.longestSentenceWordCount << " words):\n";
    report << doc1.longestSentence << "\n\n";
    report << "Document B longest sentence (" << doc2.longestSentenceWordCount << " words):\n";
    report << doc2.longestSentence << "\n\n";
    
    // Check if replacement word appears in analysis
    bool newWordInDoc1 = doc1.wordFrequency.find(newWord) != doc1.wordFrequency.end();
    bool newWordInDoc2 = doc2.wordFrequency.find(newWord) != doc2.wordFrequency.end();
    
    if (newWordInDoc1 || newWordInDoc2) {
        report << "REPLACEMENT WORD ANALYSIS\n";
        report << "------------------------\n";
        if (newWordInDoc1) {
            report << "'" << newWord << "' appears " << doc1.wordFrequency.at(newWord) << " times in Document A\n";
        }
        if (newWordInDoc2) {
            report << "'" << newWord << "' appears " << doc2.wordFrequency.at(newWord) << " times in Document B\n";
        }
        report << "\n";
    }
    
    report << "End of Updated Report\n";
    report.close();
    
    cout << "Updated analysis report generated: " << reportFilename << "\n";
    
    // Also display brief updated comparison on screen
    cout << "\n UPDATED COMPARISON SUMMARY:\n";
    printSeparator('-', 40);
    cout << " Document A Word Count: " << doc1.wordCount << "\n";
    cout << " Document B Word Count: " << doc2.wordCount << "\n";
    cout << " Updated Similarity: " << fixed << setprecision(2) << similarity << "%\n";
    cout << " Common Words: " << commonWords.size() << "\n";
    
    if (newWordInDoc1 || newWordInDoc2) {
        cout << "\n REPLACEMENT VERIFICATION:\n";
        if (newWordInDoc1) {
            cout << " '" << newWord << "' found " << doc1.wordFrequency.at(newWord) << " times in updated " << file1 << "\n";
        }
        if (newWordInDoc2) {
            cout << " '" << newWord << "' found " << doc2.wordFrequency.at(newWord) << " times in updated " << file2 << "\n";
        }
    }
}