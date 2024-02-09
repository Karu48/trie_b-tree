#include <iostream>
#include <vector>

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
                node->down = new TrieNode(word[0]);
                word.erase(0, 1);
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
            TrieNode *last = temp;

            while (word != ""){

                if (temp == nullptr){
                    bool r;

                    if (last != nullptr){
                        if (last->right == temp){
                            r = true;
                        } else {
                            r = false;
                        }
                    }

                    // If tree is empty, add the remaining of the word
                    temp = new TrieNode(word[0]);
                    word.erase(0, 1);

                    // If root is null, assign the newly created node to the root
                    if (root == nullptr){
                        root = temp;
                    } else {
                        if (r){
                            last->right = temp;
                        } else {
                            last->down = temp;
                        }
                    }
                    recursiveInsert(word, temp);
                    return;

                } else if (temp->data == word[0]){
                    // If the letter of the node matches the letter to insert, jump to down node and go to next letter in the word
                    last = temp;
                    temp = temp->down;
                    word.erase(0, 1);

                } else if (temp->data != word[0]) {
                    // If letter of the node doesn't match, jump to node on the 
                    last = temp;
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
                    word.erase(0, 1);

                } else if (temp->data != word[0]){
                    // If letter doesn't match node, go to right node
                    temp = temp->right;
                }
            }
        }

        void deleteWord(string word){
            vector<TrieNode*> letters;
            TrieNode *temp = root;

            while (word != ""){
                if (temp == nullptr){
                    // If the word is not found, return
                    return;
                }

                // Save word path to vector
                letters.push_back(temp);

                if (temp->data == word[0]){
                    // If the letter to find matches, then go to next letter and go to the down node
                    temp = temp->down;
                    word.erase(0, 1);

                } else if (temp->data != word[0]){
                    // If letter doesn't match node, go to right node
                    temp = temp->right;
                }
            }

            int i = letters.size() - 1;

            while (true){
                if (letters[i] == root){
                    // If the node to delete is the root, then first redirect the root pointer to the node on the right
                    root = root->right;
                    delete letters[i];
                    return;
                } else if (letters[i]->right != nullptr){
                    if (letters[i-1]->down == letters[i]){
                        // If parent node is up, parent's right child becomes down child
                        letters[i-1]->down = letters[i-1]->right;
                        delete letters[i];
                        return;

                    } else if (letters[i-1]->right == letters[i] && letters[i-1]->down == nullptr){
                        // If parent node if left, parent's right child becomes this node's right child
                        letters[i-1]->right = letters[i]->right;
                        delete letters[i];
                        return;
                    }
                } else if (letters[i]->down == nullptr && letters[i]->right == nullptr){
                    if (letters[i-1]->right == letters[i]){
                        letters[i-1]->right = nullptr;
                    } else {
                        letters[i-1]->down = nullptr;
                    }
                    delete letters[i];
                    i--;
                } else {
                    return;
                }


            }
        }
};

int main(){
    Trie t;
    t.insertWord("hello");
    t.insertWord("hola");
    t.insertWord("haz");
    t.insertWord("simba");
    t.insertWord("sintesis");
    t.insertWord("simbe");
    cout << t.searchWord("hello") << endl;
    cout << t.searchWord("hola") << endl;
    cout << t.searchWord("haz") << endl;
    cout << t.searchWord("simba") << endl;
    cout << t.searchWord("sintesis") << endl;
    cout << t.searchWord("simbe") << endl;
    cout << t.searchWord("hella") << endl;
    t.deleteWord("simba");
    t.deleteWord("hola");
    return 0;
}