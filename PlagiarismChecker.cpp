// Minahil Kashif
// 23i-0554
// CS-F

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const int MAX_WORDS = 1000; // max words in a document
const int MAX_LINE_LENGTH = 1000; //max characters in a line

// Global Classes and Variables
// Use of classes is must 

// Function to read input from a file
void readInput(const char* pathofInputFile);
int getNumberOfDocument();
char* getText(int documentNumber);
// Function to remove punctuation marks from documents
void removePunctuationMarks(); 
// Function to convert all characters in documents to lowercase
void convertUpperToLowerCase();
// Function to remove stopwords from documents
void removeStopWords();
// Function to generate frequencies of each unique word in each document
void generateFrequencies(char**& uniqueWords, int& uniqueCount, int**& documentFrequency);
int getFrequency(char* word, int documentNum);   // -1 if not found
void calculateAllCosineSimilarities(double**& similarities,int** documentFrequency);
double similarityIn(int doucmentNum1, int documentNum2);

//helper functions
void copyString(char* newString, const char* old) 
{
	int i = 0;
	while (old[i] != '\0') 
	{
		newString[i] = old[i];
		i++;
	}
	newString[i] = '\0';
}
int getStringLength(const char* arr) 
{
	int length = 0;
	while (arr[length] != '\0') 
	{
		length++;
	}
	return length;
}
char** convertFileToCstring(const char* pathofInputFile, int& numOfLines)
{
	char** wordList = nullptr;
	ifstream inputFile(pathofInputFile);

	if (!inputFile.is_open()) 
	{
		cout << "Error: Could not open the file " << pathofInputFile << endl;
		return nullptr;
	}

	// Reading file
	char tempLine[MAX_LINE_LENGTH];
	while (inputFile.getline(tempLine, MAX_LINE_LENGTH))
	{
		char** newWordList = new char* [numOfLines + 1 + 1];  // +1 for null terminator
		for (int i = 0; i < numOfLines; ++i)
		{
			newWordList[i] = wordList[i];  // copy old words
		}

		int lineLength = getStringLength(tempLine); // line length

		newWordList[numOfLines] = new char[lineLength + 1]; // new line

		copyString(newWordList[numOfLines], tempLine); // copy tempLine into newWordList line

		newWordList[numOfLines +1] = nullptr; //null terminate it

		/*for (int i = 0; i < numOfLines; ++i)
		{
			delete[] wordList[i];
		}
		delete[] wordList;*/

		wordList = newWordList;  // update wordList

		numOfLines++;
	}
	inputFile.close();

	return wordList;
}
char** convertDocToCstring(const char* pathofInputFile, int& numOfLines)
{
	char** wordList = nullptr;
	ifstream inputFile(pathofInputFile);

	if (!inputFile.is_open())
	{
		cout << "Error: Could not open the file " << pathofInputFile << endl;
		return nullptr;
	}
	char tempLine[MAX_LINE_LENGTH];

	while (true) {
		// Read the line
		inputFile.getline(tempLine, MAX_LINE_LENGTH);

		// Calculate the actual length of the line read
		int lineLength = getStringLength(tempLine);

		if (inputFile.fail() && !inputFile.eof()) 
		{
			cout << "Warning: A line was truncated because it exceeds the maximum line length of "
				<< MAX_LINE_LENGTH << " characters.\n";

			// Manually discard the rest of the line
			inputFile.clear(); // Clear the fail state
			char discard;
			while (inputFile.get(discard) && discard != '\n' && discard != EOF);

			// The line was truncated, so treat it as if it had MAX_LINE_LENGTH - 1 characters
			tempLine[MAX_LINE_LENGTH - 1] = '\0';
			lineLength = MAX_LINE_LENGTH - 1;
		}

		// Process the line
		char** newWordList = new char* [numOfLines + 2];  // +1 for new line, +1 for null terminator
		for (int i = 0; i < numOfLines; ++i) 
		{
			newWordList[i] = wordList[i];  // copy old words
		}

		newWordList[numOfLines] = new char[lineLength + 1]; // new line

		copyString(newWordList[numOfLines], tempLine); // copy tempLine into newWordList line

		newWordList[numOfLines + 1] = nullptr; // null terminate it

		//delete[] wordList;  // deallocate old word list
		wordList = newWordList;  // update wordList

		numOfLines++;

		// Check if end of file
		if (inputFile.eof()) {
			break;
		}
	}
	inputFile.close();

	return wordList;
}
int countWords(const char* line, int lineLength)
{
	int count = 0;
	bool inWord = false;

	for (int i = 0; i <= lineLength; i++)
	{
		if (line[i] != ' ' && line[i] != '\0') // Check if we are in a word
		{
			if (!inWord)
			{
				inWord = true;
				count++; // Start of a new word
			}
		}
		else
		{
			inWord = false; // We are outside a word
		}
		if (line[i] == '\0')
		{
			return count;
		}
	}

	return count;
}
bool areStringsEqual(char* word, char* stopWord) 
{
	int i = 0;

	while (word[i] != '\0' && stopWord[i] != '\0') 
	{
		if (word[i] != stopWord[i]) 
		{
			return false;
		}
		i++;
	}

	return word[i] == '\0' && stopWord[i] == '\0';
}


class Storage {
private:
	char** words; //array of char* pointers to store the entire content of the file//words** is the file//words* is one line
	int numOfLines; //characters in a line or wordList
	int* lineLengths; //array to store ength of each line
public:
	Storage(char** wordList = nullptr, int lineNum = 0) : numOfLines(lineNum), lineLengths(nullptr)
	{
		if (wordList == nullptr)
		{
			words = wordList;
		}
		else
		{
			words = new char* [lineNum];
			lineLengths = new int[lineNum];

			for (int i = 0; i < lineNum+1; i++)
			{
				if (wordList[i] == nullptr)
				{
					words[i] = nullptr;
					lineLengths[i] = 0;
					continue;
				}
				int wordLength = getStringLength(wordList[i]);

				words[i] = new char[wordLength + 1];
				copyString(words[i], wordList[i]);
				lineLengths[i] = wordLength;

			}
		}
		for (int i = 0; i < numOfLines; i++)
		{
			cout << "Line: "<<i<<' '<< wordList[i]<< endl;
		}
	}
	~Storage() {
		if (words != nullptr)
		{
			for (int i = 0; i < numOfLines; ++i)
			{
				delete[] words[i];  // Free word
			}
			delete[] words;  // Free word array
		}
		if (lineLengths != nullptr)
		{
			delete[] lineLengths;
		}
	}
	char** getWords() const
	{
		return words;
	}
	int getNumOfLines() const 
	{
		return numOfLines;
	}
	int* getLineLengths() const 
	{
		return lineLengths;
	}
	int getLineLength(int lineIndex) const 
	{
		if (lineIndex < 0 || lineIndex >= numOfLines) {
			std::cout << "Error: Line index " << lineIndex << " is out of range." << std::endl;
			return -1;
		}
		return lineLengths[lineIndex];
	}
	void setWords(char** newWordList, int* newLineLengths, int lineNum)
	{
		//words = newWordList;
		words = newWordList;
		//recheck lengths
		for (int i = 0; i < lineNum; i++)
		{
			int calculatedLength = getStringLength(newWordList[i]);
			if (calculatedLength != newLineLengths[i])
			{
				newLineLengths[i] = calculatedLength;
			}
		}
		lineLengths = newLineLengths;
		numOfLines = lineNum;

	}
	void setNumOfLines(const int lines)
	{
		numOfLines = lines;
	}
};
Storage* input;

//class to organize any input.txt files
class InputOrganizer {
private:
	char* stopWordsLine;
	char** stopWordsArray;
	int numStopWords;
	int numberOfDocuments;
	char** documentAddresses;
public:
	InputOrganizer() :stopWordsLine(nullptr), stopWordsArray(nullptr), numStopWords(0),
		numberOfDocuments(0), documentAddresses(nullptr) {}
	~InputOrganizer()
	{
		if (stopWordsLine)
		{
			delete[] stopWordsLine;
		}
		if (stopWordsArray)
		{
			for (int i = 0; i < numStopWords; i++)
			{
				delete[] stopWordsArray[i];
			}
			delete[] stopWordsArray;
		}
		if (documentAddresses)
		{
			for (int i = 0; i < numberOfDocuments; i++)
			{
				delete[] documentAddresses[i];
			}
			delete[] documentAddresses;
		}
	}
	InputOrganizer(Storage* storage) : numberOfDocuments(0)
	{
		int i = 0;
		int lengthAfterColonFound =0;
		char* stopWordsLine;
		char** tempWords = storage->getWords();
		//calculate characters of stop words
		bool colonFound = false;;
		while (tempWords[0][i] != '\0')
		{
			if (colonFound)
			{
				lengthAfterColonFound++;
			}
			if (tempWords[0][i] == ':')
			{
				colonFound = true;
			}
			i++;
		}
		stopWordsLine = new char[lengthAfterColonFound + 1];
		i = 0;
		int j = 0;
		colonFound = false;
		//store stop words
		while (tempWords[0][i] != '\0') 
		{
			if (colonFound) {
				stopWordsLine[j] = tempWords[0][i];
				j++;
			}
			if (tempWords[0][i] == ':') {
				colonFound = true;  // copy after colon is found
			}
			i++;
		}
		stopWordsLine[j] = '\0';
		cout << "Stop words: " << stopWordsLine << endl;
		stopWordsArray = extractStopWords(stopWordsLine);

		i = 0;
		// convert number of documents from char to int
		while (tempWords[1][i] != '\0')
		{
			numberOfDocuments = numberOfDocuments * 10 + (tempWords[1][i] - '0');
			i++;
		}

		int numLines = 0;
		while (tempWords[2 + numLines] != nullptr) 
		{
			numLines++;
		}
		documentAddresses = new char* [numLines];
		//copy each Line
		for (int i = 0; i < numLines; ++i) 
		{
			int lineLength = getStringLength(tempWords[2 + i]);
			documentAddresses[i] = new char[lineLength + 1];
			copyString(documentAddresses[i], tempWords[2 + i]);
		}
		// Print to verify the result
		for (int line = 0; line < numLines; ++line) 
		{
			std::cout << "Document address " << line << ": " << documentAddresses[line] << std::endl;
		}
	}
	int getNumberOfDocuments() const
	{
		return numberOfDocuments;
	}
	char** getStopWords() const
	{
		return stopWordsArray;
	}
	int getNumStopWords()const
	{
		return numStopWords;
	}
	char** getDocumentAddresses() const
	{
		return documentAddresses;
	}
	char** extractStopWords(char* stopWords)
	{
		const char* stopWordsLine = stopWords;
		int lineLength = input->getLineLength(0);
		numStopWords = countWords(stopWordsLine, input->getLineLength(0));
		char** wordArray = new char* [numStopWords];

		int wordStartIndex = -1;
		int wordEndIndex = -1;
		bool inWord = false;
		int wordIndex = 0;

		for (int i = 0; i < lineLength; i++)
		{
			if (stopWordsLine[i] != ' ' && !inWord) 
			{
				inWord = true;
				wordStartIndex = i;
			}
			else if ((stopWordsLine[i] == ' ' || stopWordsLine[i + 1] == '\0') && inWord)
			{
				inWord = false;
				wordEndIndex = (stopWordsLine[i] == ' ') ? i - 1 : i;

				int wordLength = wordEndIndex - wordStartIndex + 1;
				wordArray[wordIndex] = new char[wordLength + 1];
				
				for (int j = 0; j < wordLength; j++)
				{
					wordArray[wordIndex][j] = stopWordsLine[wordStartIndex + j];
				}
				wordArray[wordIndex][wordLength] = '\0';

				wordIndex++;
			}
		}
		return wordArray;
	}
};
InputOrganizer* inputOrganized;

//class to work with document.txt files
class Document {
private:
	Storage** docs;
	int numberOfDocs;
	char** uniqueWordsArr;
	int uniqueCountDoc;
	int** docFrequency;
public:
	Document()
	{
		uniqueWordsArr = nullptr;
		uniqueCountDoc = 0;
		numberOfDocs = inputOrganized->getNumberOfDocuments();
		docs = new Storage* [numberOfDocs];

		for (int i = 0; i < numberOfDocs; i++)
		{
			int numOfLines = 0;
			char** wordList = convertDocToCstring(inputOrganized->getDocumentAddresses()[i], numOfLines);
			
			// Ensure wordList is null-terminated
        char** nullTerminatedWordList = new char*[numOfLines + 1];
        for (int j = 0; j < numOfLines; j++) 
        {
            nullTerminatedWordList[j] = wordList[j];
        }
        nullTerminatedWordList[numOfLines] = nullptr;

        docs[i] = new Storage(nullTerminatedWordList, numOfLines);

			//docs[i] = new Storage(wordList, numOfLines);

			for (int j = 0; j < numOfLines; j++) 
			{
				//delete[] wordList[j];
			}
			//delete[] wordList;
		}
	}
	~Document()
	{
		if(docs)
		{
			for (int i = 0; i < numberOfDocs; ++i) 
			{
				delete docs[i];
			}
			delete[] docs;
		}
	}
	Storage* getStorage(int documentIndex) const 
	{
		if (documentIndex < 0 || documentIndex >= numberOfDocs) 
		{
			cout << "Error: Document index " << documentIndex << " is out of range." << endl;
			return nullptr;
		}
		return docs[documentIndex];
	}
	int getUniqueCount() const
	{
		return uniqueCountDoc;
	}
	char** getUniqueWordsArr() const
	{
		return uniqueWordsArr;
	}
	int** getDocumentFrequency() const
	{
		return docFrequency;
	}
	void removePunctuations()
	{
		for (int i = 0; i < numberOfDocs; i++) 
		{
			char** lines = docs[i]->getWords(); //recieve the content of the entire doc
			if (!lines)
			{
				continue;
			}
			int numOfLines = docs[i]->getNumOfLines();
			char** newWordList = new char* [docs[i]->getNumOfLines()];
			int* newLineLengths = new int[numOfLines];
			
			for (int j = 0; lines[j] != nullptr; j++) //traverse over each line and remove punctuation
			{
				int numOfLetters = 0;
	
				for (int k = 0; k < docs[i]->getLineLength(j); k++)
				{
					if ((lines[j][k] >= 'A' && lines[j][k] <= 'Z') || 
						(lines[j][k] >= 'a' && lines[j][k] <= 'z') ||
						lines[j][k]==' ' || lines[j][k] =='\n' || lines[j][k]=='\0')
					{
						if (lines[j][k] != ' ' || numOfLetters > 0)
						{
							numOfLetters++;
						}
					}
				}
				newWordList[j] = new char[numOfLetters + 1];
				newLineLengths[j] = numOfLetters;

				int wordListLength = 0;
				bool lastCharWasSpace = false;
				for (int k = 0; k < docs[i]->getLineLength(j); k++)
				{
					if ((lines[j][k] >= 'A' && lines[j][k] <= 'Z') ||
						(lines[j][k] >= 'a' && lines[j][k] <= 'z'))
					{
						newWordList[j][wordListLength++] = lines[j][k];
						lastCharWasSpace = false;
					}
					else if (lines[j][k] == ' ' && !lastCharWasSpace && wordListLength > 0)
					{
						newWordList[j][wordListLength++] = ' ';
						lastCharWasSpace = true;
					}
				}
				newWordList[j][wordListLength] = '\0'; // Null-terminate the string

			}
			docs[i]->setWords(newWordList, newLineLengths, numOfLines);
		}
	}
	void lowerCaseEmAll()
	{
		for (int i = 0; i < numberOfDocs; i++)
		{
			char** lines = docs[i]->getWords(); //recieve the content of the entire doc
			if (!lines)
			{
				continue;
			}
			
			int numOfLines = docs[i]->getNumOfLines();

			for (int j = 0; j < numOfLines; j++) //traverse over each line and remove punctuation
			{
				for (int k = 0; k < docs[i]->getLineLength(j); k++)
				{
					if (lines[j][k] >= 'A' && lines[j][k] <= 'Z')
					{
						lines[j][k] += ('a' - 'A');
					}
				}
			}
		}
	}
	void removeStopWords()
	{
		char** stopWordsArr = inputOrganized->getStopWords();
	
		for (int i = 0; i < numberOfDocs; i++)
		{
			char** lines = docs[i]->getWords(); // Receive the content of the entire doc
			if (!lines)
			{
				continue; // Skip if no content
			}
	
			int numOfLines = docs[i]->getNumOfLines();
			char** newWordList = new char* [numOfLines];
			int* newLineLengths = new int[numOfLines];
	
			//if (!newWordList || !newLineLengths)
			//{
			//	delete[] newWordList;
			//	delete[] newLineLengths;
			//	return;
			//}
	
			char** newNewWordList = new char* [numOfLines];
			//if (!newNewWordList)
			//{
			//	delete[] newWordList;
			//	delete[] newLineLengths;
			//	delete[] newNewWordList;
			//	// Handle allocation failure
			//	return;
			//}
	
			for (int j = 0; j < numOfLines; j++)
			{
				int lineLength = docs[i]->getLineLength(j);
				int numStopWords = inputOrganized->getNumStopWords();
				int newLineLength = 0;
	
				newWordList[j] = new char[lineLength + 1];
				//if (!newWordList[j])
				//{
				//	// Handle allocation failure
				//	for (int k = 0; k <= j; k++)
				//	{
				//		delete[] newWordList[k];
				//	}
				//	delete[] newWordList;
				//	delete[] newLineLengths;
				//	delete[] newNewWordList;
				//	return;
				//}
	
				int wordStart = 0;
				for (int k = 0; k <= lineLength; k++)
				{
					if (k == lineLength || lines[j][k] == ' ')
					{
						if (k > wordStart)
						{
							bool isStopWord = false;
							for (int l = 0; l < numStopWords; l++)
							{
								int stopWordLength = getStringLength(stopWordsArr[l]);
								if (k - wordStart == stopWordLength)
								{
									bool match = true;
									for (int m = 0; m < stopWordLength; m++)
									{
										if (lines[j][wordStart + m] != stopWordsArr[l][m])
										{
											match = false;
											break;
										}
									}
									if (match)
									{
										isStopWord = true;
										break;
									}
								}
							}
	
							if (!isStopWord)
							{
								for (int m = 0; m < k - wordStart; m++)
								{
									newWordList[j][newLineLength++] = lines[j][wordStart + m];
								}
								newWordList[j][newLineLength++] = ' ';
							}
						}
						wordStart = k + 1;
					}
				}
				if (newWordList[j][newLineLength - 1] == ' ')
				{
					newLineLength--;
				}
				newWordList[j][newLineLength] = '\0'; // Null terminate the string
				newLineLengths[j] = newLineLength;
	
				newNewWordList[j] = new char[newLineLength + 1];
				//if (!newNewWordList[j])
				//{
				//	// Handle allocation failure
				//	for (int k = 0; k <= j; k++)
				//	{
				//		delete[] newWordList[k];
				//		delete[] newNewWordList[k];
				//	}
				//	delete[] newWordList;
				//	delete[] newLineLengths;
				//	delete[] newNewWordList;
				//	return;
				//}
				
				for (int k = 0; k < newLineLength; k++)
				{
					newNewWordList[j][k] = newWordList[j][k];
				}
				newNewWordList[j][newLineLength] = '\0';
			}
			cout << "NEW WORD LIST " << newNewWordList << endl;
			// Update the document with the new content
			docs[i]->setWords(newNewWordList, newLineLengths, numOfLines);
	
			// Free allocated memory for the current document
			if(newWordList!=nullptr)
			{
				for (int j = 0; j < numOfLines; j++)
				{
					delete[] newWordList[j];
				}
				delete[] newWordList;
			}
		}
	}
	 //Function to count words and frequencies
	void extractUniqueWords(char**& uniqueWords, int& uniqueCount, int**& documentFrequency)
	{
		uniqueWords = nullptr;
		uniqueCount = 0;
		documentFrequency = nullptr;
	
		char** tempUniqueWords = nullptr;
		int tempUniqueCount = 0;
		int** tempDocumentFrequency = nullptr;
	
		documentFrequency = new int* [numberOfDocs];
	
		for (int i = 0; i < numberOfDocs; i++)
		{
			char** lines = docs[i]->getWords(); // Receive the content of the entire doc
			if (!lines)
			{
				continue;
			}
			int numOfLines = docs[i]->getNumOfLines();
	
			for (int j = 0; j < numOfLines; j++) // Traverse over each line
			{
				int lineLength = docs[i]->getLineLength(j);
	
				int wordStartIndex = -1;
				int wordEndIndex = -1;
				bool inWord = false;
	
				for (int k = 0; k <= lineLength; k++) 
				{
					if (lines[j][k] != ' ' && lines[j][k] != '\0' && !inWord)
					{
						inWord = true;
						wordStartIndex = k;
					}
					else if ((lines[j][k] == ' ' || lines[j][k] == '\0') && inWord)
					{
						inWord = false;
						wordEndIndex = k - 1;
	
						int wordLength = wordEndIndex - wordStartIndex + 1;
						char* word = new char[wordLength + 1];
	
						for (int l = 0; l < wordLength; l++)
						{
							word[l] = lines[j][wordStartIndex + l];
						}
						word[wordLength] = '\0';
	
						// Check if the word is unique
						bool isUnique = true;
						int wordIndex = -1;
						for (int m = 0; m < tempUniqueCount; m++)
						{
							if (areStringsEqual(word, tempUniqueWords[m]) == 1)
							{
								isUnique = false;
								wordIndex = m;
								break;
							}
						}
	
						if (isUnique)
						{
							// Allocate new array for unique words
							char** newUniqueWords = new char* [tempUniqueCount + 1];
							for (int m = 0; m < tempUniqueCount; m++)
							{
								newUniqueWords[m] = tempUniqueWords[m];
							}
							newUniqueWords[tempUniqueCount] = word;
	
							//delete[] tempUniqueWords; // Free the old array
							tempUniqueWords = newUniqueWords;
							wordIndex = tempUniqueCount;
							tempUniqueCount++;
						}
						else
						{
							//delete[] word; // Avoid memory leak if the word is not unique
						}
					}
				}
			}
		}
	
		for (int i = 0; i < numberOfDocs; i++)
		{
			documentFrequency[i] = new int[tempUniqueCount];// ();
		}
		for (int i = 0; i < numberOfDocs; i++)
		{
			for (int j = 0; j < tempUniqueCount; j++)
			{
				documentFrequency[i][j] = 0;
			}
		}
	
		for (int i = 0; i < numberOfDocs; i++)
		{
			char** lines = docs[i]->getWords(); // Receive the content of the entire doc
			if (!lines)
			{
				continue;
			}
			int numOfLines = docs[i]->getNumOfLines();

			for (int j = 0; j < numOfLines; j++) // Traverse over each line
			{
				int lineLength = docs[i]->getLineLength(j);

				int wordStartIndex = -1;
				int wordEndIndex = -1;
				bool inWord = false;

				for (int k = 0; k <= lineLength; k++)
				{
					if (lines[j][k] != ' ' && lines[j][k] != '\0' && !inWord)
					{
						inWord = true;
						wordStartIndex = k;
					}
					else if ((lines[j][k] == ' ' || lines[j][k] == '\0') && inWord)
					{
						inWord = false;
						wordEndIndex = k - 1;

						int wordLength = wordEndIndex - wordStartIndex + 1;
						char* word = new char[wordLength + 1];

						for (int l = 0; l < wordLength; l++)
						{
							word[l] = lines[j][wordStartIndex + l];
						}
						word[wordLength] = '\0';

						// Check if the word is unique
						for (int m = 0; m < tempUniqueCount; m++)
						{
							if (areStringsEqual(word, tempUniqueWords[m]) == 1)
							{
								// Update frequency count
								documentFrequency[i][m]++;
								break;
							}
						}
						//delete[] word; // Free memory for the word af

					}
				}
			}
		}
		/*for (int i = 0; i < tempUniqueCount; i++)
		{
			cout << tempUniqueWords[i] << ": ";
			for (int j = 0; j < numberOfDocs; j++)
			{
				cout<<"Doc "<<j + 1 <<' ' << documentFrequency[j][i] << ' ';
			}
			cout << endl;
		}*/
	
		// Finalize the uniqueWords array and update uniqueCount
		uniqueWords = tempUniqueWords;
		uniqueCount = tempUniqueCount;
		uniqueWordsArr = uniqueWords;
		uniqueCountDoc = uniqueCount;
		docFrequency = documentFrequency; //doc frequency needs to be fixed
	}
};
Document* documents;

void readInput(const char* pathofInputFile) 
{
	// temp array
	char** wordList = nullptr;
	int numOfLines = 0;

	wordList = convertFileToCstring(pathofInputFile, numOfLines);
	if (wordList == nullptr)
	{
		inputOrganized = new InputOrganizer();
		return;
	}
	// Update global Storage object
	Storage* newInput = new Storage(wordList, numOfLines);

	input = newInput;

	for (int i = 0; i < numOfLines; ++i) 
	{
		//delete[] wordList[i];
	}
	//delete[] wordList;

	//Organizing the input stored in storage
	InputOrganizer* tempInputOrganized = new InputOrganizer(input);
	if (inputOrganized != nullptr)
	{
		//delete inputOrganized;
		inputOrganized = nullptr;
	}
	inputOrganized = tempInputOrganized;

	documents = new Document;
	std::cout << "File read successfully." << std::endl;
}

int getNumberOfDocument() 
{
	return inputOrganized->getNumberOfDocuments();
}

char* getText(int documentNumber)
{
	// Check if the document number is within the valid range
	if (documentNumber < 1 || documentNumber > inputOrganized->getNumberOfDocuments())
	{
		cout << "Error: Document number " << documentNumber << " is out of range." << endl;
		return nullptr;
	}

	// Get the corresponding Storage object from the Document object
	Storage* storage = documents->getStorage(documentNumber - 1);  // -1 because documentNumber is 1-based

	if (!storage)
	{
		cout << "Error: No storage found for document number " << documentNumber << "." << endl;
		return nullptr;
	}

	// Get the words (lines) from the Storage object
	char** words = storage->getWords();

	if (!words)
	{
		cout << "Error: No words found for document number " << documentNumber << "." << endl;
		return nullptr;
	}

	// Calculate the total length needed for the final string
	int totalLength = 0;
	int numLines = storage->getNumOfLines();
	for (int i = 0; i < numLines; ++i)
	{
		totalLength += getStringLength(words[i]) + 1; // +1 for the space or newline
	}

	// Allocate memory for the final string
	char* result = new char[totalLength + 1];
	result[0] = '\0'; // Initialize the result as an empty string

	// Concatenate all lines into the final string
	for (int i = 0; i < numLines; ++i)
	{
		copyString(result + getStringLength(result), words[i]); // Append the line
		//result[getStringLength(result)] = ' '; // Add a space or newline (optional)
	}

	result[totalLength] = '\0'; // Null-terminate the final string

	cout << result << endl;

	return result; // Return the final concatenated string
}

void removePunctuationMarks()
{
	documents->removePunctuations();
}

void convertUpperToLowerCase()
{
	documents->lowerCaseEmAll();
}
void removeStopWords()
{
	documents->removeStopWords();
}

void generateFrequencies(char**& uniqueWords, int& uniqueCount, int**& documentFrequency)
{
	documents->extractUniqueWords(uniqueWords,uniqueCount,documentFrequency);
}

int getFrequency(char* word, int documentNum)
{
	int count = documents[0].getUniqueCount();
	char** arr = documents[0].getUniqueWordsArr();
	int** freq = documents[0].getDocumentFrequency();
	int found = false;

	for (int i = 0; i < count; i++)
	{
		int wordLength = getStringLength(word);
		int uniqueWordLength = getStringLength(arr[i]);
		if (wordLength != uniqueWordLength)
		{
			continue;
		}
		bool match = true;
		for (int j = 0; j < wordLength; j++)
		{
			if (arr[i][j] != word[j])
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			return freq[documentNum - 1][i];//freq[i][i]; //this is wrong
		}
	}

	if(!found)
		return -1;
}

double** cosineSimilarities;
void calculateAllCosineSimilarities(double**& similarities, int** documentFrequency)
{
	int uniqueCount = documents[0].getUniqueCount();
	int numOfDocs = getNumberOfDocument();

	similarities = new double* [numOfDocs];
	for (int i = 0; i< numOfDocs; i++)
	{
		similarities[i] = new double[numOfDocs];
	}
	for (int i = 0; i < numOfDocs; i++)
	{
		for (int j = 0; j < numOfDocs; j++)
		{
			similarities[i][j] = 0;
		}
	}

	double* magnitudes = new double[numOfDocs];
	double magnitudeProduct = 1;
	for (int i = 0; i < numOfDocs; i++)
	{
		double squareTotal = 0;
		for (int j = 0; j < uniqueCount; j++)
		{
			squareTotal += pow(documentFrequency[i][j], 2);
		}
		magnitudes[i] = pow(squareTotal, 0.5);
		magnitudes[i] = round(magnitudes[i] * 100) / 100.0;
	}

	for (int i = 0; i < numOfDocs; i++)
	{
		for (int j = i; j < numOfDocs; j++) // Start from 'i' to avoid duplicate calculations
		{
			// Compute dot product of document i and document j
			int dotProduct = 0;
			for (int k = 0; k < uniqueCount; k++)
			{
				dotProduct += documentFrequency[i][k] * documentFrequency[j][k];
			}

			// Calculate cosine similarity
			double cosineSimilarity = dotProduct / (magnitudes[i] * magnitudes[j]);
			cosineSimilarity = std::round(cosineSimilarity * 100.0) / 100.0;

			cosineSimilarity *= 100;

			// Store similarity in the matrix
			similarities[i][j] = cosineSimilarity;
			similarities[j][i] = cosineSimilarity; // Since similarity is symmetric
		}
	}
	cosineSimilarities = similarities;
	// Cleanup
	//delete[] magnitudes;
}

double similarityIn(int doucmentNum1, int documentNum2)
{
	return cosineSimilarities[doucmentNum1 - 1][documentNum2 - 1];

}