#include <iostream>
#include<cstdlib>
#include<cstdio>
#include <cstring>

using namespace std;

struct Node
{
    int startSubString;
    int startSuffix;
    Node **children;

//constructor to initialize all attributes for each node
    Node(int startSubString, int startSuffix)
    {
        int n = 27; //size of (alphabet size + $ char)
        children = new Node *[n];
        this->startSubString = startSubString; // the start index of the original string
        this->startSuffix = startSuffix; // the id of the suffix
        for (int i = 0; i < 27; ++i)
        { // set the children with null at the first
            children[i] = NULL;
        }
    }
    ~Node()
    {
        for(int i =0 ;i<27; i++)
        {
            delete []  children[i] ;

        }
        delete []  children ;
    }

};

class SuffixTree
{
    Node *root;
    const char *myString;
    int len; // the length of the string
public:
    SuffixTree(const char *myString)
    {//constructor to initialize all SuffixTree attributes
        this->myString = myString;
        this->root = new Node(-1, -1);
        len = strlen(myString);
        for (int i = 0; i < len; ++i)
        {
            build(i, i, root); //put all suffix strings in SuffixTree by build each of its suffix
        }
    }

    // to build each suffix
    void build(int originalStart, int startSubString, Node *node)
    {
        char newChar = myString[startSubString];
        int idx;
        if (newChar == '$') //to make '$' in range from 0 to 26
            idx = 26;
        else
            idx = newChar - 'a';

        if (node->children[idx] == nullptr) { // don't find this character branch out from root
            if (root == node)
                node->children[idx] = new Node(originalStart, originalStart); //in case my parent is original root
            else
                node->children[idx] = new Node(startSubString, originalStart);//in case my parent was not the original root
        }
        else
        {// find this character branch out from root
            int oldIdx = node->children[idx]->startSubString;
            int newIdx = startSubString;
            int length;
            //calculate the length of substring in this node
            if (node->children[idx]->startSuffix !=-1)
            { //this node is leaf so the length that we will search in it = len- id of the suffix
                length = len - node->children[idx]->startSubString;
            }
            else
            {//this node is internal node so the length that we will search in it = the minimum of the start of the children nodes - start of the original string
                length = findLessStartIdx(node->children[idx]) - node->children[idx]->startSubString;
            }
            int counter = 0;
            // find the longest common equal subString
            for (int i = oldIdx; counter < length;++counter )
            {
                if (myString[i] != myString[newIdx])
                { //we have different character then we should split
                    spilt(i, newIdx, originalStart, node->children[idx]);
                    break;
                }
                ++newIdx,++i;

            }
            if (counter == length)
            { //all character are the same in this edge and current suffix didn't end
                build(originalStart, newIdx, node->children[idx]); //go to next child and repeat build operation
            }
        }
    }


    long long findLessStartIdx(Node *node)
    { //calculate minimum start in my child to calculate length
        long long mn = 1e18;
        for (int i = 0; i < 27; ++i)
        {
            if (node->children[i] != NULL)
            { //traverse all children and minimize of start index
                mn = min(mn, (long long) node->children[i]->startSubString);
            }
        }
        return mn;
    }

    void spilt(int startOld, int startNew, int originalStart, Node *node)
    {
//to split we will create 2 new nodes
// the newOldNode to carry the rest of the old node that we spilt from it
        Node *newOldNode = new Node(startOld,node->startSuffix);// it carries the new start (old start + transfer) and the same startSuffix
// the newNewNode to carry the new node information
        Node *newNewNode = new Node(startNew,originalStart);// it carries the new start ( start + transfer) and the startSuffix(it is  originalStart to keep it if we go into more than branch)
        node->startSuffix = -1; // the old node if it was a leaf (startSuffix !=-1), now it is a leaf so its startSuffix is -1
// loop on the children of each node to set them
        for (int i = 0; i < 27; ++i)
        {
            newOldNode->children[i] = node->children[i]; // the newOldNode will take the old node children
            newNewNode->children[i] = NULL; // the new node its children will be null (leaf)
            node->children[i] = NULL; // we will set its children with null then set in them the 2 new nodes
        }

        char nextChar = myString[startOld]; // the next char of the newOld node
        int newOldIdx;
        // to handel if the next char is $
        if (nextChar == '$')
            newOldIdx = 26;
        else
            newOldIdx = nextChar - 'a';

        char nextChar1 = myString[startNew]; // the next char of the newNew node
        int newNewIdx;
        // to handel if the next char is $
        if (nextChar1 == '$')
            newNewIdx = 26;
        else
            newNewIdx = nextChar1 - 'a';


        // set the 2 new nodes at the old node children
        node->children[newNewIdx] = newNewNode;
        node->children[newOldIdx] = newOldNode;
    }

    void dfs(Node *currentSubNode)
    {
        // if currentSubNode is leaf node print the id
        if (currentSubNode->startSuffix != -1)
        {
            cout <<" " << currentSubNode->startSuffix << " ";
        }
        for (int i = 0; i < 27; ++i)
        {
            // if currentSubNode->children[i] not null traverse to get the ids.
            if (currentSubNode->children[i] != nullptr)
                dfs(currentSubNode->children[i]);
        }
    }

    void subSearch(const char *searchString, int cnt, Node *currentSubNode)
    {
        // length of search string.
        int searchLen = strlen(searchString);
        // ascii
        int index = searchString[cnt] - 'a';
        if(searchString[cnt]=='$')
            index=26;
        // if the node is leaf or the character is not found.
        if (currentSubNode->children[index] == nullptr)
        {
            cout << " Not found";
            return;
        }
        long long subStringLength;
        // if currentSubNode is not internal node.
        if (currentSubNode->children[index]->startSuffix != -1)
            //length of substring = string length - start index of substring
            subStringLength = len - currentSubNode->children[index]->startSubString;
        else
            //length of substring = minimum start index of my children nodes -  start index of substring
            subStringLength = findLessStartIdx(currentSubNode->children[index])- currentSubNode->children[index]->startSubString;

        int currentCharInd = currentSubNode->children[index]->startSubString;
        for (int i = 0; i < subStringLength; ++i)
        {
            // if the character in search string not found at original string return.
            if (myString[currentCharInd] != searchString[cnt])
            {
                cout << " Not found";
                return;
            }
            // increase the index of search string to get next character
            cnt++;
            ++currentCharInd;
            // if last index = length of search string break.
            if(cnt==searchLen)
                break;
        }
        //  if last index = length of search string break traverse from this node to get all ids(first occurrence in the string).
        if (cnt == searchLen)
        {
            dfs(currentSubNode->children[index]);
            return;
        }
        else
            // search again from the index
            subSearch(searchString, cnt, currentSubNode->children[index]);
    }
    void Search(const char *searchString)
    {
        subSearch(searchString,0,root);
    }
    ~SuffixTree()
    {
        delete myString;
    }
};

int main()
{
// Construct a suffix tree containing all suffixes of "bananabanaba$"
    cout<<"Test 1 \n";
//                     0123456789012
    SuffixTree t1("bananabanaba$");
    t1.Search("abann"); //0 6 10
    cout<<'\n';
    t1.Search("aba"); // Prints: 1 3 7
    cout<<'\n';
    t1.Search("naba");// Prints: 4 8

    cout<<"\n\nTest 2: \n";
//                     012345678
    SuffixTree t2("dodohdoh$");
    t2.Search("dodo");// 0
    cout<<'\n';
    t2.Search("hdoh");// 4
    cout<<'\n';
    t2.Search("oh");// 3 6

    cout<<"\n\nTest 3: \n";
//                     0123456789012
    SuffixTree t3("havanabanana$");
    t3.Search("naxm"); // not found;
    cout<<'\n';
    t3.Search("ava");//1
    cout<<'\n';
    t3.Search("$");//12

    cout<<"\n\nTest 4: \n";
//                     01234567
    SuffixTree t4("aramarv$");
    t4.Search("rv$");//5
    cout<<'\n';
    t4.Search("ara$");//not found
    cout<<'\n';
    t4.Search("ma");//3

    cout<<"\n\nTest 5: \n";
//                     0123456789
    SuffixTree t5("cttattaac$");
    t5.Search("tta"); // 1 4
    cout<<'\n';
    t5.Search("tatt"); // 2
    cout<<'\n';
    t5.Search("at");//3

    cout<<"\n\nTest 6: \n";
//                     01234567890
    SuffixTree t6("abcdefghab$");
    t6.Search("ab"); // 0 8
    cout<<'\n';
    t6.Search("defghab$");//3
    cout<<'\n';
    t6.Search("abc"); // 0

    cout<<"\n\nTest 7: \n";
//                     01234567890
    SuffixTree t7("ttaagacatg$");
    t7.Search("gac"); // 4
    cout<<'\n';
    t7.Search("catg$");//6
    cout<<'\n';
    t7.Search("catg&"); // not found

    cout<<"\n\nTest 8: \n";
//                     0123456789012
    SuffixTree t8("abakanabakan$");
    t8.Search("e");//not found
    cout<<'\n';
    t8.Search("bak");// 1 7
    cout<<'\n';
    t8.Search("kan"); // 3 9;

    cout<<"\n\nTest 9: \n";
//                     0123456789
    SuffixTree t9("gtgatctcg$");
    t9.Search("tctc");//4
    cout<<'\n';
    t9.Search("gtg");//0
    cout<<'\n';
    t9.Search("t");// 1 4 6

    cout<<"\n\nTest 10: \n";
//                     012345678901
    SuffixTree t10("aacgcgcacg$");
    t10.Search("cg");// 8 4 2
    cout<<'\n';
    t10.Search("acg");// 1 7
    cout<<'\n';
    t10.Search("t"); // not found.

    cout<<"\n\nTest 11: \n";
//                     0123456
    SuffixTree t11("marwa$");
    t11.Search("marwa$");//0
    cout<<'\n';
    t11.Search("cg");// not found
    cout<<'\n';
    t11.Search("$");// 5
    cout<<'\n';
    t11.Search("m"); // 0

// Add test cases here.

    return 0;
}
