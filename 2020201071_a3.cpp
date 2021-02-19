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
};

class BPlusTree{

    Node* root;
    public:
        BPlusTree(){
            root=NULL;
        }
        void insert(int);
        bool search(int key);
        void helperInsertInternal(Node* current, Node* child, int value);
        Node* getParent(Node* root, Node* currNode);
        int getCount(int key);
        int rangeQuery(int a, int b);
};

void BPlusTree::insert(int key){
    // cout << "\n inserting " << key << endl;
    if(root==NULL){
        root = new Node;
        root->is_leaf = true;
        root->key[0] = key;
        root->sizeOfNode=1;
        root->count_keys[key]++;
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

        if(currNode->count_keys.find(key) != currNode->count_keys.end()){
                currNode->count_keys[key]++;
                // cout << "Key already Inserted\n";
                return;
        }   

        if(currNode->sizeOfNode < max_keys){

            // unordered_map<int, int> m2 = currNode->count_keys;
            

            int i;
            for(i=0; i<currNode->sizeOfNode; i++){
                if(currNode->key[i] >= key)
                    break;
            }
            for(int index = currNode->sizeOfNode; index>i; index--){
                currNode->key[index] = currNode->key[index-1];
            }
            currNode->key[i] = key;

            currNode->sizeOfNode+=1;
            int s = currNode->sizeOfNode;
            currNode->ptr[s] = currNode->ptr[s-1];
            currNode->ptr[s-1] = NULL;
            currNode->count_keys[key]++;  
            // cout << "here\n";
            // for (auto i : currNode->count_keys){
            //     cout << i.first << " : " << i.second << endl;    
            // }          
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
            newLeafNode->sizeOfNode = (max_keys+1 -((max_keys+1)/2));

            int i;
            
            for(i=0;i<currNode->sizeOfNode; i++){
                currNode->key[i] = allKeys[i];
                if(allKeys[i]==key){
                    currNode->count_keys[key]++;
                }
            }
            for(int j=0;j<newLeafNode->sizeOfNode;j++){
                newLeafNode->key[j] = allKeys[i];
                
                if(allKeys[i]!=key){
                    newLeafNode->count_keys[allKeys[i]] = currNode->count_keys[allKeys[i]];
                    currNode->count_keys.erase(allKeys[i]);
                }
                else
                    newLeafNode->count_keys[allKeys[i]]++;
                i++;
            }

            // cout << "here2\n";
            // for (auto i : currNode->count_keys){
            //     cout << i.first << " : " << i.second << endl;    
            // }  

            // cout << "here3\n";
            // for (auto i : newLeafNode->count_keys){
            //     cout << i.first << " : " << i.second << endl;    
            // }  

            currNode->ptr[currNode->sizeOfNode] = newLeafNode;
            newLeafNode->ptr[newLeafNode->sizeOfNode] = currNode->ptr[max_keys];
            currNode->ptr[max_keys] = NULL;
            newLeafNode->is_leaf = true;

            // cout << "printing ptrs :\n";
            // for(int i=0;i<=currNode->sizeOfNode; i++){
            //     if(currNode->ptr[i]!=NULL)
            //         cout << "yeh yaha i hai : " << i << endl;
            // }

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

bool BPlusTree:: search(int key){
    if(!root){
        cout << "B+ Tree is empty\n";
        return false;
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
            // cout << "key found\n";
            return true;
        }
    }
    // cout << "key not found\n";
    return false;
}

int BPlusTree :: getCount(int key){
    if(!root){
        cout << "B+ Tree is empty/n";
        return INT_MIN;
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

    // int ind=0;
    // cout << "here\n";
    // for (auto i : currNode->count_keys){
    //     cout << i.first << " : " << i.second << endl;    
    // }

    for(int i=0;i<currNode->sizeOfNode;i++){
        // cout << "keys : " << currNode->key[i] << endl;
        if(currNode->key[i]==key){
            // cout << "key found\n";
            return (currNode->count_keys)[key];
        }
    }
    // cout << "key not found\n";
    return 0;
}

int BPlusTree :: rangeQuery(int a, int b){
    if(!root){
        cout << "B+ Tree is empty\n";
        return INT_MIN;
    }

    Node* currNode = root;
    while(!currNode->is_leaf){
        currNode = currNode->ptr[0];
    }
    // cout << "size of ode : " << currNode->sizeOfNode << endl;
    int count=0, i=0;
    // cout << "in range queries : " << endl;
    // for(int j=0;j<=currNode->sizeOfNode; j++){
    //     cout << "i : " << j << endl;
    //     cout << currNode->key[j] <<endl;
    //     cout << currNode->ptr[j] << endl;
    //     cout << endl;
    // }

    // cout << "\nafs\n";
    i=0;
    while(currNode->ptr[currNode->sizeOfNode]!=NULL){
        // cout << "inside while \n";
        // cout << currNode->key[i] << endl;
        if(currNode->key[i]>=a && currNode->key[i]<=b)
            count += currNode->count_keys[currNode->key[i]];
        i++;
        if(i==currNode->sizeOfNode){
            i=0;
            currNode = currNode->ptr[currNode->sizeOfNode];
        }
    }
    Node* lastLeafNode = currNode;

    for(int i=0; i<lastLeafNode->sizeOfNode;i++){
        if(lastLeafNode->key[i]>=a && lastLeafNode->key[i]<=b)
            count += lastLeafNode->count_keys[lastLeafNode->key[i]];
    }
    return count;
}

int main(int argc, char *argv[]){
    
    BPlusTree bplustree;
    
    string ipFile = argv[1];
    string opFile = "output.txt";
    // cout << "input filename : " << ipFile << endl;
    string query;
    fstream f;
    fstream f2;
    f2.open(opFile, ios::out);
    f.open(ipFile, ios::in);
    string word;
    bool res = true;

    int count, range;
    string a,b;

    while(getline (f, query)){
        // cout << query << endl;
        istringstream q(query);
        while(q >> word){
            // cout << "word : " << word << endl;
            if(word=="INSERT"){
                q >> word;
                bplustree.insert(stoi(word));
            }
            else if(word=="FIND"){
                q >> word;
                res = bplustree.search(stoi(word));
                if(res)
                    f2 << "YES\n";
                else
                    f2 << "NO\n";
            }
            else if(word=="COUNT"){
                q >> word;
                count = bplustree.getCount(stoi(word));
                f2 << to_string(count)+"\n";
            }
            else if(word=="RANGE"){
                q >> a;
                q >> b;
                range = bplustree.rangeQuery(stoi(a), stoi(b));
                f2 << to_string(range)+"\n";
            }
            else{
                f2 << "invalid query : sytax errror\n";
            }
        }
    }

    f.close();
    f2.close();

     
    // while()
    // BPlusTree bplustree;
    // bplustree.insert(1);
    // bplustree.insert(2);
    // bplustree.insert(3);
    // bplustree.insert(3);
    // bplustree.insert(3);
    // bplustree.insert(5);
    // bplustree.insert(3);
    // bplustree.insert(4);
    // bplustree.search(9);
    // cout << "count : " <<  bplustree.getCount(6) << endl;
    // bplustree.insert(9);
    // bplustree.search(6);
    // cout << "count : " <<  bplustree.getCount(6) << endl;
    // cout << "count : " << bplustree.getCount(3) << endl;
    // cout << "count : " << bplustree.getCount(5) << endl;

    // cout << bplustree.rangeQuery(2,9);
    return 0;
}