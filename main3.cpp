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
    double readabilityScore;
    int paragraphCount;
    map<int, int> wordLengthDistribution;
    double lexicalDiversity;
    vector<string> sentences;
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
set<string> findExclusiveWords(const set<string>& words1, const set<string>& words2);
pair<string, int> findLongestSentence(const string& text);
double jaccardSimilarity(const set<string>& set1, const set<string>& set2);
double calculateReadabilityScore(const DocumentStats& doc);
double calculateLexicalDiversity(const DocumentStats& doc);
map<int, int> getWordLengthDistribution(const vector<string>& tokens);
int countParagraphs(const string& text);
void generateWordCloud(const map<string, int>& wordFreq, const string& filename);
void createVisualization(const DocumentStats& doc1, const DocumentStats& doc2);
string getReadabilityLevel(double score);
map<string, pair<int, int>> getCommonWordsWithCounts(const DocumentStats& doc1, const DocumentStats& doc2);
void printCommonWordsAnalysis(const DocumentStats& doc1, const DocumentStats& doc2);
void printAdvancedAnalysis(const DocumentStats& doc1, const DocumentStats& doc2);
void printHeader();
void printSeparator(char ch = '=', int length = 80);
void printComparisonTable(const DocumentStats& doc1, const DocumentStats& doc2, double similarity);
void writeReportToFile(const DocumentStats& doc1, const DocumentStats& doc2, double similarity, const set<string>& commonWords);
DocumentStats analyzeDocument(const string& filename);

int main() {
    printHeader();
    
    string file1, file2;
    
    cout << "\n📁 Enter the path to the first document: ";
    getline(cin, file1);
    
    cout << "📁 Enter the path to the second document: ";
    getline(cin, file2);
    
    cout << "\n🔄 Processing documents...\n";
    printSeparator('-', 50);
    
    // Analyze both documents
    DocumentStats doc1 = analyzeDocument(file1);
    DocumentStats doc2 = analyzeDocument(file2);
    
    if (doc1.wordCount == 0 || doc2.wordCount == 0) {
        cout << "❌ Error: Could not process one or both documents.\n";
        return 1;
    }
    
    // Calculate similarity
    double similarity = jaccardSimilarity(doc1.uniqueWords, doc2.uniqueWords);
    
    // Display results
    printComparisonTable(doc1, doc2, similarity);
    
    // Common words analysis
    printCommonWordsAnalysis(doc1, doc2);
    
    // Advanced Analysis
    printAdvancedAnalysis(doc1, doc2);
    
    // Generate visualizations
    createVisualization(doc1, doc2);
    
    // Find common words
    set<string> commonWords = findCommonWords(doc1.uniqueWords, doc2.uniqueWords);
    
    // Write detailed report
    writeReportToFile(doc1, doc2, similarity, commonWords);
    
    cout << "\n✅ Analysis complete! Detailed report saved to 'result.txt'\n";
    printSeparator();
    
    return 0;
}

string loadFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "❌ Error: Cannot open file '" << filename << "'\n";
        return "";
    }
    
    string content, line;
    while (getline(file, line)) {
        content += line + " ";
    }
    file.close();
    
    cout << "✅ Successfully loaded: " << filename << "\n";
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

set<string> findExclusiveWords(const set<string>& words1, const set<string>& words2) {
    set<string> exclusive;
    set_difference(words1.begin(), words1.end(),
                  words2.begin(), words2.end(),
                  inserter(exclusive, exclusive.begin()));
    return exclusive;
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
    stats.paragraphCount = countParagraphs(content);
    stats.avgSentenceLength = calculateAvgSentenceLength(stats.wordCount, stats.sentenceCount);
    stats.uniqueWords = getUniqueWords(tokens);
    stats.wordFrequency = getWordFrequency(tokens);
    stats.topWords = getTopFrequentWords(stats.wordFrequency);
    stats.wordLengthDistribution = getWordLengthDistribution(tokens);
    stats.readabilityScore = calculateReadabilityScore(stats);
    stats.lexicalDiversity = calculateLexicalDiversity(stats);
    
    // Store sentences for analysis
    istringstream iss(content);
    string sentence;
    char c;
    while (iss.get(c)) {
        sentence += c;
        if (c == '.' || c == '!' || c == '?') {
            if (!sentence.empty()) {
                stats.sentences.push_back(sentence);
            }
            sentence.clear();
        }
    }
    if (!sentence.empty()) {
        stats.sentences.push_back(sentence);
    }
    
    auto longestInfo = findLongestSentence(content);
    stats.longestSentence = longestInfo.first;
    stats.longestSentenceWordCount = longestInfo.second;
    
    return stats;
}

void printHeader() {
    printSeparator();
    cout << setw(40) << "📊 TEXT COMPARATOR 📊\n";
    cout << setw(42) << "Document Analysis & Comparison Tool\n";
    printSeparator();
}

void printSeparator(char ch, int length) {
    cout << string(length, ch) << "\n";
}

void printComparisonTable(const DocumentStats& doc1, const DocumentStats& doc2, double similarity) {
    cout << "\n📈 COMPARISON RESULTS\n";
    printSeparator('-', 80);
    
    cout << left << setw(25) << "Metric" 
         << setw(25) << "Document A" 
         << setw(25) << "Document B" << "\n";
    printSeparator('-', 75);
    
    cout << left << setw(25) << "📄 Filename:" 
         << setw(25) << doc1.filename.substr(0, 22) 
         << setw(25) << doc2.filename.substr(0, 22) << "\n";
    
    cout << left << setw(25) << "📝 Word Count:" 
         << setw(25) << doc1.wordCount 
         << setw(25) << doc2.wordCount << "\n";
    
    cout << left << setw(25) << "📋 Sentence Count:" 
         << setw(25) << doc1.sentenceCount 
         << setw(25) << doc2.sentenceCount << "\n";
    
    cout << left << setw(25) << "🔤 Unique Words:" 
         << setw(25) << doc1.uniqueWords.size() 
         << setw(25) << doc2.uniqueWords.size() << "\n";
    
    cout << left << setw(25) << "📏 Avg Sentence Length:" 
         << setw(25) << fixed << setprecision(2) << doc1.avgSentenceLength 
         << setw(25) << doc2.avgSentenceLength << "\n";
    
    cout << left << setw(25) << "📐 Longest Sentence:" 
         << setw(25) << doc1.longestSentenceWordCount << " words"
         << setw(25) << doc2.longestSentenceWordCount << " words" << "\n";
    
    printSeparator('-', 75);
    
    cout << "\n🎯 SIMILARITY ANALYSIS\n";
    printSeparator('-', 30);
    cout << "🔗 Jaccard Similarity: " << fixed << setprecision(2) << similarity << "%\n";
    
    set<string> commonWords = findCommonWords(doc1.uniqueWords, doc2.uniqueWords);
    cout << "🤝 Common Words: " << commonWords.size() << "\n";
    
    cout << "\n🏆 TOP 5 FREQUENT WORDS\n";
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
        cout << "❌ Error: Cannot create result.txt\n";
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
    
    // Exclusive words
    set<string> exclusive1 = findExclusiveWords(doc1.uniqueWords, doc2.uniqueWords);
    set<string> exclusive2 = findExclusiveWords(doc2.uniqueWords, doc1.uniqueWords);
    
    report << "Words exclusive to Document A: " << exclusive1.size() << "\n";
    report << "Words exclusive to Document B: " << exclusive2.size() << "\n\n";
    
    report << "LONGEST SENTENCES\n";
    report << "-----------------\n\n";
    report << "Document A longest sentence (" << doc1.longestSentenceWordCount << " words):\n";
    report << doc1.longestSentence << "\n\n";
    report << "Document B longest sentence (" << doc2.longestSentenceWordCount << " words):\n";
    report << doc2.longestSentence << "\n\n";
    
    report << "End of Report\n";
    report.close();
}

// UNIQUE ADVANCED FEATURES

double calculateReadabilityScore(const DocumentStats& doc) {
    // Improved Flesch Reading Ease Score with better syllable counting
    if (doc.sentenceCount == 0 || doc.wordCount == 0) return 50.0;
    
    double avgWordsPerSentence = static_cast<double>(doc.wordCount) / doc.sentenceCount;
    
    // Better syllable counting
    double totalSyllables = 0;
    for (const auto& wordPair : doc.wordFrequency) {
        const string& word = wordPair.first;
        int frequency = wordPair.second;
        
        // Improved syllable counting
        int syllables = 0;
        bool previousWasVowel = false;
        
        for (char c : word) {
            bool isVowel = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y');
            if (isVowel && !previousWasVowel) {
                syllables++;
            }
            previousWasVowel = isVowel;
        }
        
        // Handle silent e
        if (word.length() > 2 && word.back() == 'e' && syllables > 1) {
            syllables--;
        }
        
        // Every word has at least 1 syllable
        if (syllables == 0) syllables = 1;
        
        totalSyllables += syllables * frequency;
    }
    
    double avgSyllablesPerWord = totalSyllables / doc.wordCount;
    
    // Flesch Reading Ease formula: 206.835 - (1.015 × ASL) - (84.6 × ASW)
    double score = 206.835 - (1.015 * avgWordsPerSentence) - (84.6 * avgSyllablesPerWord);
    
    // Clamp between 0-100
    return max(0.0, min(100.0, score));
}

double calculateLexicalDiversity(const DocumentStats& doc) {
    // Type-Token Ratio (TTR)
    if (doc.wordCount == 0) return 0.0;
    return static_cast<double>(doc.uniqueWords.size()) / doc.wordCount * 100.0;
}

map<int, int> getWordLengthDistribution(const vector<string>& tokens) {
    map<int, int> distribution;
    for (const string& word : tokens) {
        distribution[word.length()]++;
    }
    return distribution;
}

int countParagraphs(const string& text) {
    int count = 1; // Start with 1 paragraph
    for (size_t i = 0; i < text.length() - 1; i++) {
        if (text[i] == '\n' && text[i + 1] == '\n') {
            count++;
        }
    }
    return count;
}

string getReadabilityLevel(double score) {
    if (score >= 90) return "Very Easy (5th grade) 📗";
    else if (score >= 80) return "Easy (6th grade) 📘";
    else if (score >= 70) return "Fairly Easy (7th grade) 📙";
    else if (score >= 60) return "Standard (8th-9th grade) 📕";
    else if (score >= 50) return "Fairly Difficult (10th-12th grade) 📔";
    else if (score >= 30) return "Difficult (College level) 📓";
    else return "Very Difficult (Graduate level) 📚";
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
    cout << "\n🤝 COMMON WORDS DETAILED ANALYSIS\n";
    printSeparator('=', 70);
    
    auto commonWords = getCommonWordsWithCounts(doc1, doc2);
    
    if (commonWords.empty()) {
        cout << "❌ No common words found between the documents.\n";
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
    
    cout << "\n📊 Common Words Statistics:\n";
    cout << "• Total common words: " << commonWords.size() << "\n";
    cout << "• Most shared word: '" << sortedCommon[0].first 
         << "' (used " << (sortedCommon[0].second.first + sortedCommon[0].second.second) << " times total)\n";
    
    // Calculate vocabulary overlap
    double overlapPercentage = (static_cast<double>(commonWords.size()) / 
                               max(doc1.uniqueWords.size(), doc2.uniqueWords.size())) * 100;
    cout << "• Vocabulary overlap: " << fixed << setprecision(1) << overlapPercentage << "%\n";
}

void generateWordCloud(const map<string, int>& wordFreq, const string& filename) {
    // Generate ASCII word cloud representation
    ofstream cloud(filename + "_wordcloud.txt");
    if (!cloud.is_open()) return;
    
    cloud << "=== WORD CLOUD VISUALIZATION ===" << "\n\n";
    
    vector<pair<string, int>> words(wordFreq.begin(), wordFreq.end());
    sort(words.begin(), words.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    
    // Create visual representation
    for (size_t i = 0; i < min(size_t(20), words.size()); i++) {
        string word = words[i].first;
        int freq = words[i].second;
        
        // Make word size proportional to frequency
        if (freq >= 5) {
            transform(word.begin(), word.end(), word.begin(), ::toupper);
        }
        
        cloud << word;
        for (int j = 0; j < freq && j < 10; j++) {
            cloud << "*";
        }
        cloud << " (" << freq << ")\n";
    }
    
    cloud.close();
}

void createVisualization(const DocumentStats& doc1, const DocumentStats& doc2) {
    // Generate word clouds
    generateWordCloud(doc1.wordFrequency, "doc1");
    generateWordCloud(doc2.wordFrequency, "doc2");
    
    // Create comparison chart
    ofstream chart("comparison_chart.txt");
    if (chart.is_open()) {
        chart << "=== VISUAL COMPARISON CHART ===\n\n";
        
        // Word count bars
        chart << "Word Count Comparison:\n";
        chart << "Document A: ";
        for (int i = 0; i < min(doc1.wordCount / 5, 50); i++) chart << "█";
        chart << " (" << doc1.wordCount << ")\n";
        
        chart << "Document B: ";
        for (int i = 0; i < min(doc2.wordCount / 5, 50); i++) chart << "█";
        chart << " (" << doc2.wordCount << ")\n\n";
        
        // Readability comparison
        chart << "Readability Score Comparison:\n";
        chart << "Document A: ";
        for (int i = 0; i < static_cast<int>(doc1.readabilityScore / 2); i++) chart << "▓";
        chart << " (" << fixed << setprecision(1) << doc1.readabilityScore << ")\n";
        
        chart << "Document B: ";
        for (int i = 0; i < static_cast<int>(doc2.readabilityScore / 2); i++) chart << "▓";
        chart << " (" << fixed << setprecision(1) << doc2.readabilityScore << ")\n";
        
        chart.close();
    }
}

void printAdvancedAnalysis(const DocumentStats& doc1, const DocumentStats& doc2) {
    cout << "\n🧠 ADVANCED LINGUISTIC ANALYSIS\n";
    printSeparator('=', 80);
    
    cout << left << setw(35) << "📊 Metric" 
         << setw(22) << "Document A" 
         << setw(23) << "Document B" << "\n";
    printSeparator('-', 80);
    
    cout << left << setw(35) << "📚 Readability Score:" 
         << setw(22) << (to_string(static_cast<int>(doc1.readabilityScore)) + "/100")
         << setw(23) << (to_string(static_cast<int>(doc2.readabilityScore)) + "/100") << "\n";
    
    cout << left << setw(35) << "📖 Reading Level:" 
         << setw(22) << getReadabilityLevel(doc1.readabilityScore).substr(0, 21)
         << setw(23) << getReadabilityLevel(doc2.readabilityScore).substr(0, 22) << "\n";
    
    cout << left << setw(35) << "🎯 Lexical Diversity (TTR):" 
         << setw(22) << (to_string(static_cast<int>(doc1.lexicalDiversity)) + "%")
         << setw(23) << (to_string(static_cast<int>(doc2.lexicalDiversity)) + "%") << "\n";
    
    cout << left << setw(35) << "📄 Paragraph Count:" 
         << setw(22) << doc1.paragraphCount 
         << setw(23) << doc2.paragraphCount << "\n";
    
    // Calculate average word lengths properly
    double avgLen1 = 0.0, avgLen2 = 0.0;
    for (const auto& pair : doc1.wordFrequency) {
        avgLen1 += pair.first.length() * pair.second;
    }
    for (const auto& pair : doc2.wordFrequency) {
        avgLen2 += pair.first.length() * pair.second;
    }
    avgLen1 /= doc1.wordCount;
    avgLen2 /= doc2.wordCount;
    
    cout << left << setw(35) << "📏 Average Word Length:" 
         << setw(22) << (to_string(avgLen1).substr(0, 4) + " letters")
         << setw(23) << (to_string(avgLen2).substr(0, 4) + " letters") << "\n";
    
    // Calculate sentence complexity
    double sentenceComplexity1 = doc1.avgSentenceLength;
    double sentenceComplexity2 = doc2.avgSentenceLength;
    
    cout << left << setw(35) << "🔗 Sentence Complexity:" 
         << setw(22) << (sentenceComplexity1 > 20 ? "High 🔴" : 
                         sentenceComplexity1 > 15 ? "Medium 🟡" : "Low 🟢")
         << setw(23) << (sentenceComplexity2 > 20 ? "High 🔴" : 
                         sentenceComplexity2 > 15 ? "Medium 🟡" : "Low 🟢") << "\n";
    
    printSeparator('-', 80);
    
    // Document comparison insights
    cout << "\n🔍 DOCUMENT INSIGHTS\n";
    printSeparator('-', 50);
    
    if (doc1.readabilityScore > doc2.readabilityScore + 5) {
        cout << "📖 Document A is significantly easier to read\n";
    } else if (doc2.readabilityScore > doc1.readabilityScore + 5) {
        cout << "📖 Document B is significantly easier to read\n";
    } else {
        cout << "📖 Both documents have similar reading difficulty\n";
    }
    
    if (doc1.lexicalDiversity > doc2.lexicalDiversity + 5) {
        cout << "🎯 Document A has richer vocabulary diversity\n";
    } else if (doc2.lexicalDiversity > doc1.lexicalDiversity + 5) {
        cout << "🎯 Document B has richer vocabulary diversity\n";
    } else {
        cout << "🎯 Both documents have similar vocabulary richness\n";
    }
    
    if (doc1.avgSentenceLength > doc2.avgSentenceLength + 3) {
        cout << "📝 Document A uses more complex sentence structures\n";
    } else if (doc2.avgSentenceLength > doc1.avgSentenceLength + 3) {
        cout << "📝 Document B uses more complex sentence structures\n";
    } else {
        cout << "📝 Both documents have similar sentence complexity\n";
    }
    
    cout << "\n📈 VISUALIZATION FILES GENERATED:\n";
    cout << "• doc1_wordcloud.txt - Word frequency visualization for Document A\n";
    cout << "• doc2_wordcloud.txt - Word frequency visualization for Document B\n";
    cout << "• comparison_chart.txt - Visual comparison charts\n";
}