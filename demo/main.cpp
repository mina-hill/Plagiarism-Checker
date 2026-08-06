// Reference driver for PlagiarismChecker.cpp
//
// PlagiarismChecker.cpp defines the whole processing pipeline as free
// functions and helper classes but does not define main() itself, so it
// has no entry point on its own. This file wires the declared functions
// together in the order the pipeline expects, using the exact prototypes
// already declared at the top of PlagiarismChecker.cpp.
//
// Build (from the repo root):
//   g++ PlagiarismChecker.cpp demo/main.cpp -o plagiarism_checker
//   ./plagiarism_checker samples/input.txt
//
// See the "Known Limitations" section in the README before relying on this
// for large inputs -- a couple of rough edges in the original assignment
// code are documented there rather than fixed, to keep this repo an
// accurate copy of the real submission.

#include <iostream>

void readInput(const char* pathofInputFile);
int getNumberOfDocument();
void removePunctuationMarks();
void convertUpperToLowerCase();
void removeStopWords();
void generateFrequencies(char**& uniqueWords, int& uniqueCount, int**& documentFrequency);
void calculateAllCosineSimilarities(double**& similarities, int** documentFrequency);
double similarityIn(int documentNum1, int documentNum2);

int main(int argc, char* argv[])
{
    const char* inputPath = (argc > 1) ? argv[1] : "samples/input.txt";

    readInput(inputPath);

    removePunctuationMarks();
    convertUpperToLowerCase();
    removeStopWords();

    char** uniqueWords = nullptr;
    int uniqueCount = 0;
    int** docFrequency = nullptr;
    generateFrequencies(uniqueWords, uniqueCount, docFrequency);

    double** similarities = nullptr;
    calculateAllCosineSimilarities(similarities, docFrequency);

    int n = getNumberOfDocument();
    std::cout << "\n=== Pairwise similarity ===\n";
    for (int i = 1; i <= n; i++)
    {
        for (int j = i + 1; j <= n; j++)
        {
            std::cout << "Doc " << i << " vs Doc " << j << ": "
                       << similarityIn(i, j) << "%\n";
        }
    }

    return 0;
}
