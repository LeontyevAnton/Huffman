#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#define NL "\r\n"
#else
#define NL "\n"
#endif
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <bitset>
#include <direct.h>
#include <windows.h>
#include <algorithm>

#include <experimental\filesystem>


using namespace std;
vector<string> test;
struct pnode
{
	unsigned char ch; // char
	int p;		// probability
};

static int pnode_compare(const void* elem1, const void* elem2)
{
	const pnode a = *(pnode*)elem1;
	const pnode b = *(pnode*)elem2;
	if (a.ch < b.ch) return 1; // 1 - less (reverse for decreasing sort)
	else if (a.ch > b.ch) return -1;
	return 0;
}

struct treenode : public pnode
{
	char lcode;
	char rcode;
	treenode* left; // left child
	treenode* right; // right child
};

void BubbleSort(pnode* t, int tsize) {
	vector<pair<char, int >>test;

	for (int i = 0; i < tsize; i++) {
		test.push_back(make_pair(t[i].ch, t[i].p));
	}

	pair<char, int >temp;

	for (int i = 0; i < tsize - 1; i++)
		for (int j = tsize - 1 - 1; j >= i; j--) {
			temp = test[j];

			if (test[j].second != test[j + 1].second) {
				test.erase(test.begin() + j);

				if (temp.second > test[j].second)
					test.insert(test.begin() + j, temp);
				else
					test.insert(test.begin() + (j + 1), temp);
			}
		}
	for (int i = 0; i < test.size(); i++) {
		t[i].ch = test[i].first;
		t[i].p = test[i].second;
	}

}

/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.*/
void compAndSwap(pnode* t, int i, int j, int dir) {
	if (dir == (t[i].p > t[j].p))
		swap(t[i], t[j]);
}

/*It recursively sorts a bitonic sequence in ascending order,
  if dir = 1, and in descending order otherwise (means dir=0).
  The sequence to be sorted starts at index position low,
  the parameter cnt is the number of elements to be sorted.*/
void bitonicMerge(pnode* t, int low, int cnt, int dir) {
	if (cnt > 1)
	{
		int k = cnt / 2;
		for (int i = low; i < low + k; i++)
			compAndSwap(t, i, i + k, dir);
		bitonicMerge(t, low, k, dir);
		bitonicMerge(t, low + k, k, dir);
	}
}

/* This function first produces a bitonic sequence by recursively
    sorting its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order */
void bitonicSort(pnode* t, int low, int cnt, int dir)
{
	if (cnt > 1)
	{
		int k = cnt / 2;

		// sort in ascending order since dir here is 1 
		bitonicSort(t, low, k, 1);

		// sort in descending order since dir here is 0 
		bitonicSort(t, low + k, k, 0);

		// Will merge wole sequence in ascending order 
		// since dir=1. 
		bitonicMerge(t, low, cnt, dir);
	}
}

/* Caller of bitonicSort for sorting the entire array of
   length N in ASCENDING order */
void sort(pnode* t, int N, int up)
{
	bitonicSort(t, 0, N, up);
}
class Coder
{
private:
	int tsize; // table size (number of chars)
	pnode* ptable; // table of probabilities
	unordered_map<char, string> codes; // codeword for each char

public:
	void Encode(const char* inputFilename, const char* outputFilename, int streamNumber, int sortType, int dataBlockNumber){
		unordered_map<char, int> freqs; // frequency for each char from input text
		int i;

		//  Opening input file
		//
		FILE* inputFile;
		inputFile = fopen(inputFilename, "r");
		assert(inputFile);

		//  Counting chars
		//
		char ch; // char
		while (fscanf(inputFile, "%c", &ch) != EOF)
			freqs[ch]++;

		if (sortType) {
			tsize = 256;
		}
		else {
			tsize = freqs.size();
		}


		//  Building decreasing freqs table
		//
		ptable = new pnode[tsize];
		for (int i = 0; i < tsize; i++) {
			ptable[i].ch = NULL;
			ptable[i].p = 0;
		}
		assert(ptable);
		unordered_map<char, int>::iterator fi;
		for (fi = freqs.begin(), i = 0; fi != freqs.end(); ++fi, ++i)
		{
			ptable[i].ch = (*fi).first;
			ptable[i].p = (*fi).second;// / ftot;
		}
		//Sort by chars
		qsort(ptable, tsize, sizeof(pnode), pnode_compare);
		//Sort by frequency
		if (sortType) {
			sort(ptable, tsize, 0);
		}
		else {
			BubbleSort(ptable, tsize);
		}

		EncHuffman(dataBlockNumber);

		//  Opening output file
		FILE* outputFile;
		outputFile = fopen(outputFilename, "wb");
		assert(outputFile);

		//  Outputing ptable and codes
		//printf("%i" NL, tsize);
		//fprintf(outputFile, "%i" NL, tsize);
		if (sortType) {
			printf("__________BITONIC SORT__________\n");
			fprintf(outputFile, "__________BITONIC SORT__________\n");
		}
		else {
			printf("___________BUBBLE SORT___________\n");
			fprintf(outputFile, "___________BUBBLE SORT___________\n");
		}


		int lettersCounter = 0;
		for (i = 0; i < tsize; i++)
		{
			if (ptable[i].p != 0) {
				printf("%d   %c\t%d\t%X\t%d\t%s" NL, lettersCounter + 1, ptable[i].ch, unsigned int(ptable[i].ch), unsigned int(ptable[i].ch), ptable[i].p, codes[ptable[i].ch].c_str());
				fprintf(outputFile, "%d   %c\t%d\t%X\t%d\t%s" NL, lettersCounter + 1, ptable[i].ch, unsigned int(ptable[i].ch), unsigned int(ptable[i].ch), ptable[i].p, codes[ptable[i].ch].c_str());
				lettersCounter++;
			}
		}

		//  Outputing encoded text

		fseek(inputFile, SEEK_SET, 0);
		printf(NL);
		fprintf(outputFile, NL);
		vector<string> charStream;
		vector<vector<string>> StringsArr;
		vector<string> StringsArr32;

		StringsArr.resize(streamNumber);
		StringsArr32.resize(streamNumber);

		while (fscanf(inputFile, "%c", &ch) != EOF)
			charStream.push_back(codes[ch].c_str());

		//Division into streams
		for (int j = 0; j < streamNumber; j++)
			for (int z = j; z < charStream.size(); z += streamNumber)
				StringsArr[j].push_back(charStream[z]);

		//Stream lines formation
		for (int z = 0; z < streamNumber; z++)
			for (int j = 0; j < StringsArr[z].size(); j++)
				StringsArr32[z] += StringsArr[z][j];

		//Checking strings for 32-bit multiplicity
		//for (int z = 0; z < streamNumber; z++) {
		//	if (StringsArr32[z].size() % 32 != 0)
		//		for (int k = 0; k < StringsArr32[z].size() % 32; k++)
		//			StringsArr32[z] += '0';
		//}

		//Searching the longest string
		//int maxLength = StringsArr32[0].size();
		//for (int z = 1; z < streamNumber; z++) {
		//	if (maxLength < StringsArr32[z].size())
		//		maxLength = StringsArr32[z].size();
		//}

		//Reducing strings to the same size (max)
		//for (int z = 0; z < streamNumber; z++) {
		//	int k = maxLength - StringsArr32[z].size();
		//	for (int i = 0; i < k; i++) {
		//		StringsArr32[z] += "0";
		//	}
		//}

		//Printing in columns
		//fprintf(outputFile, "________STREAM NUMBER: %d________\n", streamNumber);
		
		
		for (int z = 0; z < streamNumber; z++) {
			string tempOut;
			tempOut += StringsArr32[z];
			test[z] += tempOut;
		}
		printf(NL);
		//  Cleaning
		codes.clear();
		delete[] ptable;

		//  Closing files
		fclose(outputFile);
		fclose(inputFile);
	}

	//void Decode(const char* inputFilename, const char* outputFilename)
	//{
	//	//  Opening input file
	//	FILE* inputFile;
	//	inputFile = fopen(inputFilename, "r");
	//	assert(inputFile);
	//	//  Loading codes
	//	fscanf(inputFile, "%i", &tsize);
	//	char ch, code[128];
	//	float p;
	//	int i;
	//	fgetc(inputFile); // skip end line
	//	for (i = 0; i < tsize; i++)
	//	{
	//		ch = fgetc(inputFile);
	//		fscanf(inputFile, "%f %s", &p, code);
	//		codes[ch] = code;
	//		fgetc(inputFile); // skip end line
	//	}
	//	fgetc(inputFile); // skip end line
	//	//  Opening output file
	//	//
	//	FILE* outputFile;
	//	outputFile = fopen(outputFilename, "w");
	//	assert(outputFile);
	//	//  Decoding and outputing to file
	//	//
	//	string accum = "";
	//	unordered_map<char, string>::iterator ci;
	//	while ((ch = fgetc(inputFile)) != EOF)
	//	{
	//		accum += ch;
	//		for (ci = codes.begin(); ci != codes.end(); ++ci)
	//			if (!strcmp((*ci).second.c_str(), accum.c_str()))
	//			{
	//				accum = "";
	//				printf("%c", (*ci).first);
	//				fprintf(outputFile, "%c", (*ci).first);
	//			}
	//	}
	//	printf(NL);
	//	//  Cleaning
	//	//
	//	fclose(outputFile);
	//	fclose(inputFile);
	//}

private:
	void EncHuffman(int dataBlockNumber)
	{
		//  Creating leaves (initial top-nodes)
		treenode* n;
		vector<treenode*> tops; // top-nodes
		int i, numtop = tsize;
		for (i = 0; i < numtop; i++)
		{
			n = new treenode;
			assert(n);
			n->ch = ptable[i].ch;
			n->p = ptable[i].p;
			n->left = NULL;
			n->right = NULL;
			tops.push_back(n);
		}

		//  Building binary tree.
		//  Combining last two nodes, replacing them by new node
		//  without invalidating sort
		ofstream firstStep;

		firstStep.open("Trees\\treeBuilding_" + to_string(dataBlockNumber) + ".txt");
		while (numtop > 1)
		{
			n = new treenode;
			assert(n);
			n->p = tops[numtop - 2]->p + tops[numtop - 1]->p;
			n->left = tops[numtop - 2];
			n->right = tops[numtop - 1];
			firstStep << "\' " << tops[numtop - 2]->ch << " \'" << " (" << tops[numtop - 2]->p << ") " << "+" << "\' " << tops[numtop - 1]->ch << " \'" << " (" << tops[numtop - 1]->p << ") =" << tops[numtop - 2]->p + tops[numtop - 1]->p << endl;
			if (n->left->p < n->right->p)
			{
				n->lcode = '0';
				n->rcode = '1';
			}
			else
			{
				n->lcode = '1';
				n->rcode = '0';
			}
			tops.pop_back();
			tops.pop_back();
			bool isins = false;
			std::vector<treenode*>::iterator ti;
			for (ti = tops.begin(); ti != tops.end(); ++ti)
				if ((*ti)->p <= n->p)//Was lower
				{
					tops.insert(ti, n);
					isins = true;
					break;
				}
			if (!isins) tops.push_back(n);
			numtop--;
		}
		firstStep.close();
		//  Building codes
		treenode* root = tops[0];
		GenerateCode(root);

		//  Cleaning
		DestroyNode(root);
		tops.clear();
	}

	void GenerateCode(treenode* node) // for outside call: node is root
	{
		static string sequence = "";
		if (node->left)
		{
			sequence += node->lcode;
			GenerateCode(node->left);
		}

		if (node->right)
		{
			sequence += node->rcode;
			GenerateCode(node->right);
		}

		if (!node->left && !node->right)
			codes[node->ch] = sequence;

		int l = (int)sequence.length();
		if (l > 1) sequence = sequence.substr(0, l - 1);
		else sequence = "";
	}

	void DestroyNode(treenode* node) // for outside call: node is root
	{
		if (node->left)
		{
			DestroyNode(node->left);
			delete node->left;
			node->left = NULL;
		}

		if (node->right)
		{
			DestroyNode(node->right);
			delete node->right;
			node->right = NULL;
		}
	}
};


int show_usage() {
	printf("Huffman coding algorithm" NL);
	printf("progName.exe fileName.txt" NL);
	printf(NL);
	exit(0);
}

int main(int argc, char** argv)
{
	int dFlag = 0;
	int streamNumber = 1;
	int sortType = 0;
	int memFormat = 0;
	int packSize = 100;
	char inputFilename[128];
	char outputFilename[128];



	setlocale(LC_ALL, "Russian");

	if ((argc == 1) || (strcmp(argv[1], "-i") == 0)) {
		cout << endl << endl;
		cout << ">>------------------------------   Информационная панель  ------------------------------<<" << endl;
		cout << ">>Формат запуска программы:                                                             <<" << endl;
		cout << ">>        <progName>.exe <inputFile.txt> -param_1 -param_2 ...                          <<" << endl;
		cout << ">>Параметры:                                                                            <<" << endl;
		cout << ">>  -i       |  Информация о параметрах                                                 <<" << endl;
		cout << ">>  -s Число |  Количество потоков                                                      <<" << endl;
		cout << ">>  -t       |  Сортировка Бэтчера (по умолчанию \"пузырьком\")                          <<" << endl;
		cout << ">>  -p Число |  Количество пакетов данных                                               <<" << endl;
		cout << ">>  -m       |  Формат вывода данных в память [8 или 32 бита]  (по умолчанию 32 бита)   <<" << endl;
		cout << ">>--------------------------------------------------------------------------------------<<" << endl;
		cout << endl;
		return 0;
	}

	strcpy(inputFilename, argv[1]);

	for (uint_fast16_t i = 2; i < argc; i++) {


		if (strcmp(argv[i], "-d") == 0) {
			dFlag = 1;
		}
		if (strcmp(argv[i], "-t") == 0) {
			sortType = 1;
		}
		if (strcmp(argv[i], "-s") == 0) {
			if (argv[i + 1][0] != '-')
				sscanf_s(argv[i + 1], "%d", &streamNumber);
		}
		if (strcmp(argv[i], "-p") == 0) {
			if (argv[i + 1][0] != '-') {
				sscanf_s(argv[i + 1], "%d", &packSize);
			}
		}

		if (strcmp(argv[i], "-m") == 0) {
			memFormat = 1;
		}
	}

	//if (dFlag) {
	//	strcpy(outputFilename, "decoded.txt");
	//}
	//else {
	//strcpy(outputFilename, "encoded.txt");
	//}



	experimental::filesystem::remove_all("Trees");
	experimental::filesystem::remove_all("Standart Data Parts");
	experimental::filesystem::remove_all("Standart Data Parts Hex");
	experimental::filesystem::remove_all("Encoded Files");
	experimental::filesystem::remove_all("Out Mems");

	_mkdir("Trees");
	_mkdir("Standart Data Parts");
	_mkdir("Standart Data Parts Hex");
	_mkdir("Encoded Files");
	_mkdir("Out Mems");
	string text = "";

	ifstream fin(inputFilename);
	vector<ofstream> filePart;
	filePart.resize(1);
	filePart[0].open("Standart Data Parts\\StandartDataPart_0.txt");
	string buff;
	int temp;
	vector<vector<char>> charArray;
	charArray.resize(1);
	int fileCounter = 0;

	if (fin.is_open()) {
		while (getline(fin, buff)) {
			if (packSize > fileCounter) {
				for (int j = 0; j < buff.size(); j++)
					if (charArray[fileCounter].size() < 65536) {
						charArray[fileCounter].push_back(buff[j]);
						filePart[fileCounter] << buff[j];
					}
					else {
						filePart[fileCounter].close();
						fileCounter++;
						charArray.resize(charArray.size() + 1);
						filePart.resize(filePart.size() + 1);
						if (packSize > fileCounter)
							filePart[fileCounter].open("Standart Data Parts\\StandartDataPart_" + to_string(fileCounter) + ".txt");
						//break;
					}
			}
			else {
				break;
			}
		}
	}
	else {
		printf("Can't open File!\n");
		return 0;
	}
	fin.close();
	//vector<string>test;
	test.resize(streamNumber);
	for (int j = 0; j < fileCounter; j++) {
		ofstream fileHex;
		fileHex.open("Standart Data Parts Hex\\StandartDataHEX_" + to_string(j) + ".txt");
		for (int i = 0; i < charArray[j].size(); i++)
			fileHex << hex << ((uint32_t)charArray[j][i] & 0x000000FF) << endl;
		fileHex.close();

		string t1 = "Standart Data Parts\\StandartDataPart_" + to_string(j) + ".txt";
		strcpy(inputFilename, t1.c_str());
		string t2 = "Encoded Files\\encoded_" + to_string(j) + ".txt";
		strcpy(outputFilename, t2.c_str());
		//  Calling encoding or decoding subroutine
		Coder* coder;
		coder = new Coder;
		assert(coder);

		//if (!dFlag) {
		coder->Encode(inputFilename, outputFilename, streamNumber, sortType, j);
		//}
		//else {
		//	coder->Decode(inputFilename, outputFilename);
		//}

		delete coder;
	}
	vector<ofstream> mems;
	mems.resize(streamNumber);
	vector<string> memsOut;
	memsOut.resize(streamNumber);
	vector<int> powArr;
	powArr.resize(31);
	for (int i = 0; i < 31; i++) {
		powArr[i] = pow(2, i);
	}

	if (memFormat == 0) {

		for (int i = 0; i < streamNumber; i++) {
			int strCounter = 0;
			for (int j = 0; j < test[i].size(); j += 32) {
				for (int k = j; k < j + 32; k++) {
					if (test[i][k] != NULL)
						memsOut[i] += test[i][k];
					else
						memsOut[i] += "0";
				}
				memsOut[i] += ",\n";
				strCounter++;
			}

			for (int j = 0; j < powArr.size(); j++) {
				if (powArr[j] - strCounter > 0) {
					for (int k = 0; k < powArr[j] - strCounter - 1; k++) {
						memsOut[i] += "00000000000000000000000000000000,\n";
					}
					memsOut[i] += "00000000000000000000000000000000;\n";
					break;
				}
			}
		}
	}
	else {
		for (int i = 0; i < streamNumber; i++) {
			int strCounter = 0;
			for (int j = 0; j < test[i].size(); j += 8) {
				for (int k = j; k < j + 8; k++) {
					if (test[i][k] != NULL)
						memsOut[i] += test[i][k];
					else
						memsOut[i] += "0";
				}
				memsOut[i] += ",\n";
				strCounter++;
			}

			for (int j = 0; j < powArr.size(); j++) {
				if (powArr[j] - strCounter > 0) {
					for (int k = 0; k < powArr[j] - strCounter - 1; k++) {
						memsOut[i] += "00000000,\n";
					}
					memsOut[i] += "00000000;\n";
					break;
				}
			}
		}
	}
	for (int i = 0; i < streamNumber; i++) {
		mems[i].open("Out Mems\\mems_" + to_string(i) + ".coe");
		mems[i] << "MEMORY_INITIALIZATION_RADIX=2;\nMEMORY_INITIALIZATION_VECTOR = \n";
		mems[i] << memsOut[i];
		mems[i].close();
	}

	return 0;
}