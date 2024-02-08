#include <iostream>

using namespace std;

struct TrieNode{
    char data;
    TrieNode* right = nullptr;
    TrieNode* down = nullptr;
    TrieNode(){}
    TrieNode(char dat){
        data = dat;
    }
};

class Trie{
    private:
        TrieNode* root = nullptr;
        
        // recursive function to insert the whole word
        void recursiveInsert(string word, TrieNode* node){
            if (word != ""){
                node = new TrieNode(word[0]);
                word.erase(0);
                recursiveInsert(word, node->down) ;
            }
        }

    public:
        Trie(){}
        Trie(char dat){
            root = new TrieNode(dat);
        }
        void insertWord(string word){

            TrieNode *temp = root;

            while (word != ""){

                if (temp == nullptr){
                    // If tree is empty, add the remaining of the word
                    recursiveInsert(word, temp);

                } else if (temp->data == word[0]){
                    // If the letter of the node matches the letter to insert, jump to down node and go to next letter in the word
                    temp = temp->down;
                    word.erase(0);

                } else if (temp->data != word[0]) {
                    // If letter of the node doesn't match, jump to node on the right
                    temp = temp->right;
                }

            }
        }

        bool searchWord(string word){
            TrieNode *temp = root;

            while (true){
                if (word == ""){
                    // If all the letters have been found, then the word has been found
                    return true;
                }

                if (temp == nullptr){
                    // If temp reaches a null, then the word is not in the trie
                    return false;
                }

                if (temp->data == word[0]){
                    // If the letter to find matches, then go to next letter and go to the down node
                    temp = temp->down;
                    word.erase(0);

                } else if (temp->data != word[0]){
                    // If letter doesn't match node, go to right node
                    temp = temp->right;
                }
            }
        }

        void deleteWord(string word){

        }
};

int main(){
    return 0;
}