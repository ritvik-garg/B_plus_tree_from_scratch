#include<bits/stdc++.h>
using namespace std;

// int order = 3;
int max_keys = 2;
int max_child = 3;

class Node{
    public:
        int key[2];
        Node* ptr[3];
        int sizeOfNode=0;
        bool is_leaf = true;
        unordered_map<int, int> count_keys;
        friend class BPlusTree;
};

class BPlusTree{

    Node* root;
    public:
        BPlusTree(){
            root=NULL;
        }
        void insert(int);
        void search(int key);
        void helperInsertInternal(Node* current, Node* child, int value);
        Node* getParent(Node* root, Node* currNode);
};

void BPlusTree::insert(int key){
    if(root==NULL){
        root = new Node;
        root->is_leaf = true;
        root->key[0] = key;
        root->sizeOfNode=1;
    }
    else{
        Node* currNode = root;
        Node* parentNode;

        while(!currNode->is_leaf){
            parentNode = currNode;
            int i=0;

            while(i<currNode->sizeOfNode){
                if(key < currNode->key[i]){
                    currNode = currNode->ptr[i];
                    break;
                }
                else if (i==currNode->sizeOfNode-1){
                    currNode = currNode->ptr[i+1];
                    break;
                }
                i++;
            }
        }

        if(currNode->sizeOfNode < max_keys){

            // unordered_map<int, int> m2 = currNode->count_keys;
            if(currNode->count_keys.find(key) != currNode->count_keys.end()){
                currNode->count_keys[key]++;
                cout << "Key Inserted\n";
                return;
            }

            int i;
            for(i=0; i<currNode->sizeOfNode; i++){
                if(currNode->key[i] >= key)
                    break;
            }
            currNode->key[i] = key;
            for(int index = currNode->sizeOfNode; index>i; index--){
                currNode->key[index] = currNode->key[index-1];
            }

            currNode->sizeOfNode+=1;
            int s = currNode->sizeOfNode;
            currNode->ptr[s] = currNode->ptr[s-1];
            currNode->ptr[s-1] = NULL;
            currNode->count_keys[key]++;
            
        }
        else{
            Node* newLeafNode = new Node;
            vector<int> allKeys;

            for(int i=0;i<max_keys; i++){
                allKeys.push_back(currNode->key[i]);
            }
            allKeys.push_back(key);

            sort(allKeys.begin(), allKeys.end());

            currNode->sizeOfNode = (max_keys+1)/2;
            newLeafNode->sizeOfNode = max_keys+1 -(max_keys+1)/2;

            int i;
            for(i=0;i<currNode->sizeOfNode; i++){
                currNode->key[i] = allKeys[i];
            }
            for(int j=0;j<newLeafNode->sizeOfNode;j++){
                newLeafNode->key[j] = allKeys[i];
                i++;
            }

            currNode->ptr[currNode->sizeOfNode] = newLeafNode;
            newLeafNode->ptr[newLeafNode->sizeOfNode] = currNode->ptr[max_keys];
            currNode->ptr[max_keys] = NULL;
            newLeafNode->is_leaf = true;

            if(currNode==root){
                Node* temp = new Node;
                temp->key[0] = newLeafNode->key[0];
                temp->ptr[0] = currNode;
                temp->ptr[1] = newLeafNode;
                temp->is_leaf = false;
                temp->sizeOfNode += 1;
                root = temp;
            }
            else{
                helperInsertInternal(parentNode, newLeafNode, newLeafNode->key[0]);
            }
        }
    }
}

void BPlusTree::helperInsertInternal(Node* currNode, Node* child, int key){
    if(currNode->sizeOfNode < max_keys){
        int i;
        for(i=0; i<currNode->sizeOfNode; i++){
            if(currNode->key[i] >= key)
                break;
        }
        currNode->key[i] = key;
        for(int index = currNode->sizeOfNode; index>i; index--){
            currNode->key[index] = currNode->key[index-1];
        }
        for(int j = currNode->sizeOfNode+1; j>i;j--){
            currNode->ptr[j] = currNode->ptr[j-1];
        }

        currNode->sizeOfNode+=1;
        currNode->ptr[i+1] = child;
    }
    else{
        // line 308
        Node* newInternalNode = new Node;
        int nodeKeys[max_keys+1];
        Node* nodePtrs[max_child+1];

        for(int i=0;i<max_child; i++){
            nodePtrs[i] = currNode->ptr[i];
            if(i != max_child-1){
                nodeKeys[i] = currNode->key[i];
            }
        }
        
        int i=0;
        for(i=0; i<max_keys; i++){
            if(nodeKeys[i] > key){
                break;
            }
        }

        for(int index = max_child+1; index>i; index--){
            if(index != max_child+1)
                nodeKeys[index] = currNode->key[index-1];
            
            if(index!=i+1)
                nodePtrs[index] = currNode->ptr[index-1];
        }

        nodeKeys[i] = key;
        nodePtrs[i+1] = child;

        int newNodeSize = max_keys - (max_keys+1)/2;
        currNode->sizeOfNode = (max_keys+1)/2;
        newInternalNode->sizeOfNode = newNodeSize;
        newInternalNode->is_leaf = false;

        int j=currNode->sizeOfNode+1; // should not be +1
        i=0;

        while(true){
            if(i!=newNodeSize){
                newInternalNode->key[i] = nodeKeys[j];
            }
            if(j!=currNode->sizeOfNode){
                newInternalNode->ptr[i] = nodePtrs[j];
            }
            i++;
            j++;
            if(i>=newNodeSize+1)
                break;
        }

        if(currNode==root){
            Node* newRoot = new Node;
            newRoot->key[0] = currNode->key[currNode->sizeOfNode];// check it
            newRoot->ptr[0] = currNode;
            newRoot->ptr[1] = newInternalNode;
            newRoot->is_leaf = false;
            newRoot->sizeOfNode = 1;
            root = newRoot;
        }
        else{

            Node* parentNode = getParent(root, currNode);
            helperInsertInternal(parentNode, newInternalNode, currNode->key[currNode->sizeOfNode]);
        }
    }
}

Node* BPlusTree::getParent(Node* currNode, Node* child){
    if((currNode->ptr[0])->is_leaf==true ||  currNode->is_leaf==true){
        return NULL;
    }
    Node* parent;

    for(int i=0; i<currNode->sizeOfNode+1; i++){
        if(currNode->ptr[i]==child){
            parent = currNode;
            return parent;
        }
        parent = getParent(currNode->ptr[i], child);

        if(parent!=NULL)
            return parent;
    }
    return parent;
}

void BPlusTree:: search(int key){
    if(!root){
        cout << "B+ Tree is empty\n";
        return;
    }

    Node* currNode = root;
    while(!currNode->is_leaf){
        for(int i=0; i<currNode->sizeOfNode; i++){
            if(currNode->key[i] > key){
                currNode = currNode->ptr[i];
                break;
            }
            if(i==currNode->sizeOfNode-1){
                currNode = currNode->ptr[i+1];
                break;
            }
        }
    }

    for(int i=0;i<currNode->sizeOfNode;i++){
        if(currNode->key[i]==key){
            cout << "key found\n";
            return;
        }
    }
    cout << "key not found\n";
    return;
}



int main(){
    BPlusTree bplustree;
    bplustree.insert(1);
    bplustree.insert(2);
    bplustree.insert(3);
    bplustree.search(7);
    return 0;
}