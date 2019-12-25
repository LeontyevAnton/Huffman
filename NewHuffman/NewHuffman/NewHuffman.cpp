#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#define NL "\r\n"
#else
#define NL "\n"
#endif

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <bitset>
#include <algorithm>

using namespace std;

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

void BubbleSort(pnode* t,int tsize) {
	vector<pair<char, int >>test;
	
	for (int i = 0; i < tsize;i++){
		test.push_back(make_pair(t[i].ch, t[i].p));
	}
	
	pair<char, int >temp;
	cout << test.size() << endl;
	for (int i = 0; i < tsize-1; i++) 
		for (int j = tsize-1 - 1; j >= i; j--) {
		temp = test[j];
		
		test.erase(test.begin()+j);
		//for (int j = 0; j < test.size(); j++)
		//	std::cout << j << "  " << test[j].first << "   " << test[j].second << endl;
		
		if (temp.second>test[j].second) {
			test.insert(test.begin() + j, temp);
		}
		else {
			/*pair<char, int >temp1;
			temp1 = temp;
			temp = test[i];
			test[i] = temp1;*/
			test.insert(test.begin() + (j+1), temp);
		}
	
	}
	for (int i = 0; i < test.size(); i++){
		t[i].ch = test[i].first;
		t[i].p = test[i].second;
	}
	
}

/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.*/
void compAndSwap(pnode* t, int i, int j, int dir){
	if (dir == (t[i].p > t[j].p))
		swap(t[i], t[j]);
}

/*It recursively sorts a bitonic sequence in ascending order,
  if dir = 1, and in descending order otherwise (means dir=0).
  The sequence to be sorted starts at index position low,
  the parameter cnt is the number of elements to be sorted.*/
void bitonicMerge(pnode*t, int low, int cnt, int dir){
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
void bitonicSort(pnode*t, int low, int cnt, int dir)
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
void sort(pnode*t , int N, int up)
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
	void Encode(const char* inputFilename, const char* outputFilename)
	{
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
		tsize =  freqs.size();
		
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

				
		//	sort(ptable, tsize, 0);
		qsort(ptable, tsize, sizeof(pnode), pnode_compare);
		for (int i = 0; i < tsize; i++) {
			cout << ptable[i].ch << "  " << unsigned int(ptable[i].ch) << "  " << ptable[i].p << endl;
		}

		BubbleSort(ptable, tsize);
		cout << "After Bubble" << endl;
		for (int i = 0; i < tsize; i++) {
			cout << ptable[i].ch << "  " << unsigned int(ptable[i].ch) << "  " << ptable[i].p << endl;
		}

		
		EncHuffman();

		//  Opening output file
		//
		FILE* outputFile;
		outputFile = fopen(outputFilename, "wb");
		assert(outputFile);

		//  Outputing ptable and codes
		//
		printf("%i" NL, tsize);
		fprintf(outputFile, "%i" NL, tsize);
		for (i = 0; i < tsize; i++)
		{
			printf("%c\t%d\t%X\t%d\t%s" NL, ptable[i].ch, unsigned int(ptable[i].ch), unsigned int(ptable[i].ch), ptable[i].p, codes[ptable[i].ch].c_str());
			fprintf(outputFile, "%c\t%d\t%X\t%d\t%s" NL, ptable[i].ch, unsigned int(ptable[i].ch), unsigned int(ptable[i].ch), ptable[i].p, codes[ptable[i].ch].c_str());
		}

		//  Outputing encoded text
		//
		fseek(inputFile, SEEK_SET, 0);
		printf(NL);
		fprintf(outputFile, NL);
		vector<string> totalString, evenString,oddString,evenOutputString,oddOutputString;
		while (fscanf(inputFile, "%c", &ch) != EOF)
			totalString.push_back(codes[ch].c_str());

		for (int z = 0; z < totalString.size(); z++)
		{
			if (z % 2 == 0)
				evenString.push_back(totalString[z]);
			else
				oddString.push_back(totalString[z]);
		}
		string totlaEvenString,totalOddString;
		for (int i = 0; i < evenString.size(); i++) {
			totlaEvenString += evenString[i];
		}
		for (int i = 0; i < oddString.size(); i++) {
			totalOddString += oddString[i];
		}

		int max = totalOddString.size();
		if (totlaEvenString.size() > totalOddString.size())
			max = totlaEvenString.size();
		
		fprintf(outputFile, "FIRST STREAM\t\t\tSECOND STREAM\n");
		for (int i = 0; i < max; i += 32) {
			string evenNullreplace, oddNullReplace;
			for (int j = i; j < i + 32; j++) {
				if (totlaEvenString[j] == NULL)
					totlaEvenString[j] = '0';
				fprintf(outputFile, "%c", totlaEvenString[j]);
				evenNullreplace += totlaEvenString[j];
			}
			evenOutputString.push_back(evenNullreplace);
			fprintf(outputFile, "\t");
			for (int j = i; j < i + 32; j++) {
				if (totalOddString[j] == NULL)
					totalOddString[j] = '0';
				fprintf(outputFile, "%c", totalOddString[j]);
				oddNullReplace += totalOddString[j];
			}
			oddOutputString.push_back(oddNullReplace);
			fprintf(outputFile, "\n");
		}
		
		fprintf(outputFile, "HEX\n");
		fprintf(outputFile, "FIRST STREAM   SECOND STREAM\n");
		for (int i = 0; i < evenOutputString.size(); i++) {
			bitset<32> evenSet(evenOutputString[i]);
			fprintf(outputFile, "%08X", evenSet.to_ullong());
			fprintf(outputFile, "\t");
			bitset<32> oddSet(oddOutputString[i]);
			fprintf(outputFile, "%08X", oddSet.to_ullong());
			fprintf(outputFile, "\n");
		}
		
		
		printf(NL);
		//  Cleaning
		//
		codes.clear();
		delete[] ptable;

		//  Closing files
		//
		fclose(outputFile);
		fclose(inputFile);
	}

	void Decode(const char* inputFilename, const char* outputFilename)
	{
		//  Opening input file
		//
		FILE* inputFile;
		inputFile = fopen(inputFilename, "r");
		assert(inputFile);

		//  Loading codes
		//
		fscanf(inputFile, "%i", &tsize);
		char ch, code[128];
		float p;
		int i;
		fgetc(inputFile); // skip end line
		for (i = 0; i < tsize; i++)
		{
			ch = fgetc(inputFile);
			fscanf(inputFile, "%f %s", &p, code);
			codes[ch] = code;
			fgetc(inputFile); // skip end line
		}
		fgetc(inputFile); // skip end line

		//  Opening output file
		//
		FILE* outputFile;
		outputFile = fopen(outputFilename, "w");
		assert(outputFile);

		//  Decoding and outputing to file
		//
		string accum = "";
		unordered_map<char, string>::iterator ci;
		while ((ch = fgetc(inputFile)) != EOF)
		{
			accum += ch;
			for (ci = codes.begin(); ci != codes.end(); ++ci)
				if (!strcmp((*ci).second.c_str(), accum.c_str()))
				{
					accum = "";
					printf("%c", (*ci).first);
					fprintf(outputFile, "%c", (*ci).first);
				}
		}
		printf(NL);

		//  Cleaning
		//
		fclose(outputFile);
		fclose(inputFile);
	}

private:
	void EncHuffman()
	{
		//  Creating leaves (initial top-nodes)
		//
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
		//
		ofstream firstStep;
		firstStep.open("treeBuilding.txt");
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
		//
		treenode* root = tops[0];
		GenerateCode(root);

		//  Cleaning
		//
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
	int i = 1;
	int dFlag = 0;
	char inputFilename[128];
	char outputFilename[128];

	printf(NL);

	if (i == argc) show_usage();

	if (strcmp(argv[i], "-d") == 0) {
		dFlag = 1;
		i++;
		if (i == argc) show_usage();
	}

	strcpy(inputFilename, argv[i]);
	i++;

	if (i < argc) {
		strcpy(outputFilename, argv[i]);
	}
	else {
		if (dFlag) {
			strcpy(outputFilename, "decoded.txt");
		}
		else {
			strcpy(outputFilename, "encoded.txt");
		}
	}

	setlocale(LC_ALL, "Russian");
	string text = "";
	ifstream fin(inputFilename);
	ofstream filePart;
	filePart.open("StandartDataPart.txt");
	string buff;
	int temp;
	vector<char> charArray;
	/*if (fin.is_open())
		for (int i = 0; i < 65536; )
		{
			getline(fin, buff);
			filePart << buff;
			for (int j = 0; j < buff.size(); j++)
			{
				charArray.push_back(buff[j]);
				i++;
			}
		}
	fin.close();*/
	i = 0;
	if (fin.is_open()) {
		while (getline(fin, buff)) {
			for (int j = 0; j < buff.size(); j++)
				if (charArray.size() < 65536) {
					charArray.push_back(buff[j]);
					filePart << buff[j];
				}else
					break;
			
		}
	}
	else {
		printf("Can't open File!\n");
	}
	printf("CHAR ARRAY SIZE: %d\n",charArray.size());
	fin.close();
	filePart.close();
	ofstream fileHex;
	string t1 = "StandartDataPart.txt";
	fileHex.open("StandartDataHEX.txt");
	for (int i = 0; i < charArray.size(); i++)
		fileHex << hex << ((uint32_t)charArray[i] & 0x000000FF) << endl;
	fileHex.close();

	strcpy(inputFilename, t1.c_str());
	//  Calling encoding or decoding subroutine
	//
	Coder* coder;
	coder = new Coder;
	assert(coder);
	if (!dFlag) {
		coder->Encode(inputFilename, outputFilename);
	}
	else {
		coder->Decode(inputFilename, outputFilename);
	}

	delete coder;
	
	printf(NL);




	return 0;
}
