#include "main.h"
#include <memory>
int MAXSIZE;

struct Data {
    int freq = 0;
    int timestamp = 0;
};
/** Huffman tree node implement */
static long long count_rotate = 0;
class HuffNode {
private:
    char c;
    int freq;
    unique_ptr<HuffNode> left;
    unique_ptr<HuffNode> right;
public:
    friend class HuffTree;
    // DEFAULT CONSTRUCTOR
    HuffNode() : freq(0), c('\0'), left(nullptr), right(nullptr) {}
    // CONSTRUCTOR WITH PARAMETERS
    HuffNode(int frequency, char character,
             std::unique_ptr<HuffNode> leftChild = nullptr,
             std::unique_ptr<HuffNode> rightChild = nullptr)
            : freq(frequency), c(character), left(std::move(leftChild)), right(std::move(rightChild)) {}
    HuffNode(char character, int frequency,
             unique_ptr<HuffNode> leftChild,
             unique_ptr<HuffNode> rightChild)
            : c(character), freq(frequency),
              left(std::move(leftChild)), right(std::move(rightChild)) {}
    int get_height(const unique_ptr<HuffNode>& root) {
        if (!root) return 0;
        if (!root->left && !root->right) return 1;
        int hleft = get_height(root->left);
        int hright = get_height(root->right);
        return 1+ max(hleft, hright);
    }
    int get_freq() { return freq;}
    char get_c() { return c;}
};
class HuffTree {
private:
    unique_ptr<HuffNode> root;
    int get_height(const unique_ptr<HuffNode>& root) {
        if (!root) return 0;
        if (!root->left && !root->right) return 1;
        int hleft = get_height(root->left);
        int hright = get_height(root->right);
        return 1+ max(hleft, hright);
    }

    unique_ptr<HuffNode> rotateLeft(unique_ptr<HuffNode> root) {
        if (!root || !root->right) {
            // Rotation is not possible if root or root's right child is null
            cout << "MISTAKE RotaLeft";
            return root;
        }
        unique_ptr<HuffNode> newRoot = std::move(root->right);
        // Reattach the left child of the new root to the right of the original root
        root->right = std::move(newRoot->left);
        // Now, make the original root the left child of the new root
        newRoot->left = std::move(root);
        // Return the new root
        return newRoot;
    }
    unique_ptr<HuffNode> rotateRight(unique_ptr<HuffNode> root) {
        if (!root || !root->left) {
            // Rotation is not possible if root or root's right child is null
            cout << "MISTAKE RotaRight";
            return root;
        }
        unique_ptr<HuffNode> newRoot = std::move(root->left);
        // Reattach the left child of the new root to the right of the original root
        root->left = std::move(newRoot->right);
        // Now, make the original root the left child of the new root
        newRoot->right = std::move(root);
        // Return the new root
        return newRoot;
    }

    unique_ptr<HuffNode> insertRec(unique_ptr<HuffNode> &node, int n_freq, char n_c) {
        // Base case of insertion
        if (!node) return make_unique<HuffNode>(n_freq, n_c);
        if (n_freq < node->freq) node->left = insertRec(node->left, n_freq, n_c);
        else node->right = insertRec(node->right, n_freq, n_c);
        return std::move(node);
    }

    void preorderTraversal(const unique_ptr<HuffNode>& node, const function<void(char, int, int)>& visit) const
    {
        if (!node) return;

        // Call the function on the current node

        int h = node->get_height(node);
        visit(node->c, node->freq, h);

        // Traverse left subtree
        preorderTraversal(node->left, visit);

        // Traverse right subtree
        preorderTraversal(node->right, visit);
    }

    unique_ptr<HuffNode> get_unbalance_rec(unique_ptr<HuffNode> &node) {
        // Find in preorder NLR
        if(!node) return nullptr;
        int h_left = get_height(node->left);
        int h_right = get_height(node->right);
        int gap = abs(h_left - h_right);
        if (gap > 1 ) {
            // Unbalance => check unbalance case and rotate
            cout << "Unbalance at: \n";
            cout << "Node: " << node->c << " with frequency: " << node->freq  << " and height: " << node->get_height(node) << endl;
            if (h_left > h_right) { // Left subtree is higher
                int hsub_left = get_height(node->left->left);
                int hsub_right = get_height(node->left->right);
                if (hsub_left > hsub_right) { /** LL CASE*/
                    node = std::move(rotateRight(std::move(node)));
                }
                else { /** LR CASE*/
                    node->left = std::move(rotateLeft(std::move(root->left)));
                    node = std::move(rotateRight(std::move(node)));
                }
            }
            else {                  // Right subtree is higher
                int hsub_left = get_height(node->right->left);
                int hsub_right = get_height(node->right->right);
                if (hsub_right > hsub_left) {    /** RR CASE*/
                    node = std::move(rotateLeft(std::move(node)));
                }
                else { /** RL CASE*/
                    node->right = std::move(rotateRight(std::move(node->right)));
                    node = std::move(rotateLeft(std::move(node)));
                }
            }

        }

        // check Left
        unique_ptr<HuffNode> leftCheck = get_unbalance_rec(node->left);
        if (leftCheck) return leftCheck;
        // check Right
        unique_ptr<HuffNode> rightCheck = get_unbalance_rec(node->right);
        if (rightCheck) return rightCheck;

        // Tree is balance
        return nullptr;
    }

public:
    friend class HuffNode;
    // CONSTRUCTOR
    HuffTree() : root(nullptr) {}

    HuffTree(char c, int f) : root(std::make_unique<HuffNode>(f, c)) {}

    HuffTree(char c, int f, std::unique_ptr<HuffNode> l, std::unique_ptr<HuffNode> r)
            : root(std::make_unique<HuffNode>(f, c, std::move(l), std::move(r))) {}

    HuffTree(std::unique_ptr<HuffNode> node) : root(std::move(node)) {}

    int get_freq() {
        return root->get_freq();
    }

    char get_c() {
        return root->get_c();
    }

    unique_ptr<HuffNode> get_root()  { return std::move(root); }


    void set_root(unique_ptr<HuffNode> node) {root = std::move(node);}

    int get_height() { return this->get_height(root); }

    void insert (unique_ptr<HuffNode> node) {
        root = insertRec(root, node->freq, node->c);
    }
    void insert (int n_freq, char n_c) {
        root = insertRec(root, n_freq, n_c);
    }


    // Public wrapper for traversal starting from root
    void preorderTraversal(const std::function<void(char, int, int)>& visit) const {
        preorderTraversal(root, visit);
    }

    // Find unbalance node NLR
    unique_ptr<HuffNode> get_unbalance() { return get_unbalance_rec(root); }

    string to_bin(char x) {
        if(!root) return "";
        stack<string> s;
        stack<HuffNode *> t;
        t.push(root.get());
        while(!t.empty()) {
            HuffNode *temp = t.top(); t.pop();
            string st = "";
            if(!s.empty()) {
                st = s.top(); s.pop();
            }
            if(temp->get_c() == x) return st;
            if(temp->left) {
                t.push(temp->left.get());
                s.push(st + "0");
            }
            if(temp->right) {
                t.push(temp->right.get());
                s.push(st + "1");
            }
        }
        return "";
    }
};
bool cmp (const unique_ptr<HuffTree> &a, const unique_ptr<HuffTree> &b) {
    if (a->get_freq() > b->get_freq()) return false;
    else if(a->get_freq() == b->get_freq()) return a->get_c() < b->get_c();
    else return true;
}

class RESTAURANT_Gojo {
    class Tree_BST;
private:
    vector<Tree_BST*> areaTable;
public:
    RESTAURANT_Gojo() : areaTable(MAXSIZE) {}
    void insertAreaTable(int result) {
        int ID = result % MAXSIZE + 1;
        areaTable[ID]->insert(result);
        }
    void remove_KOKUSEN();
    void print_LIMITLESS(int num);
private:
    class Tree_BST {
        class Node;
    private:
        Node* root;
        queue<int> queueTime;
        void insert_rec(int result, Node* r) {
            if (!r) { r = new Node(result); return; }
            else if (result < r->result) insert_rec(result, r->left);
            else insert_rec(result, r->right);
        }
    public:
        Tree_BST(): root(nullptr) {}
        Tree_BST(int r): root(new Node(r)) {}
        int size() { return (int)queueTime.size(); }
        void insert(int result) {
            insert_rec(result, root);
        };
        void remove_rec(Node* toDelete);
        void remove();
        void print_rec(Node* node);
        void print();
    private:
        class Node {
        private:
            int result;
            Node* left;
            Node* right;
            friend class Tree_BST;
        public:
            explicit Node(int result) : result(result), left(nullptr), right(nullptr) {};
        };
    };
};
//* nhà hàng su cờ na
class RESTAURANT_Sukuna{
    class Node;
private:
    vector<Node* > areaTable;  //! nơi lưu trữ các khu vực
    list<Node* > LRU; 		  //!  Least Recently Used này là cơ chế khu vực nào có khác vào nhà hàng lâu nhất
private:



    void ReHeap_down(int index)
    {
        //TODO: với giá trị xét là areaTable[index]->head.size()
        //TODO: này là min heap nên areaTable[index]->head.size() nào bé hơn thì ở trên
        //TODO: nếu 2 thằng bằng nhau thì chọn ra khu nào có khác vào gần nhất dùng list<Node* > LRU;
        //TODO: ví dụ khu A và khu B có số khách bằng nhau nếu khu A mới có khách vào thì so sánh min heap thì khu B đang nhỏ hơn khu A nên ở trên khu A
        int numberOfElements = areaTable.size();
        if(index * 2 + 1 > numberOfElements) return;
        if(index * 2 + 2 >= numberOfElements) {
            if(areaTable[index * 2 + 1]->head.size() < areaTable[index]->head.size()) swap(areaTable[index], areaTable[index * 2 + 1]);
        }
        else {
            int child = areaTable[index * 2 + 1]->head.size() > areaTable[index * 2 + 2]->head.size() ? index * 2 + 1 : index * 2 + 2;
            if(areaTable[index]->head.size() > areaTable[child]->head.size()) {
                swap(areaTable[index], areaTable[child]);
                ReHeap_down(child);
            }
        }
    }

    void ReHeap_up(int index)
    {
        //TODO: với giá trị xét là areaTable[index].size()
        //TODO: này là min heap nên areaTable[index].size() nào bé hơn thì ở trên
        //TODO: nếu 2 thằng bằng nhau thì chọn ra khu nào có khác vào gần nhất
        //TODO: này xử lí tương tự reheap_down
        int par = (index - 1)/2;
        while(areaTable[index]->head.size() > areaTable[(par)]->head.size()) {
            swap(areaTable[index], areaTable[(par)]);
            index = par;
            par = (par - 1) / 2;
        }

    }

    //* nếu node chưa tồn tại trong LRU thì thêm vô nếu tồn tại thì dịch nó lên đầu danh sách
    void moveTop(Node* node)
    {
        //TODO: BƯỚC 1 Tìm vị trí của node trong danh sách
        bool found = false;
        int id_find = node->ID;
        for (Node* i : LRU) if(i == node) { found = true; break; }

        //TODO: BƯỚC 2 nếu nó tồn tại thì dịch nó lên đầu danh sách, nếu không thì insert ở đầu danh sách
        if (found)  LRU.remove(node);
        LRU.push_front(node);
    }

    //* xóa một node ra khỏi danh sách liên kết không gần gọi delete nha vì đã dùng bên dưới hàm xóa
    void removeNode(Node* node)
    {
        //TODO:
        auto it = find(areaTable.begin(), areaTable.end(), node);
        if (it != areaTable.end()) areaTable.erase(it);
        auto it1 = find(LRU.begin(), LRU.end(), node);
        if (it1 != LRU.end()) LRU.erase(it1);
    }
public:
    RESTAURANT_Sukuna(){}

    void insertAreaTable(int result)
    {
        int ID = result % MAXSIZE + 1;
        //*bước 1: kiểm tra xem heap có đang quản lí khu ID hay không nếu chưa quản lí thì phải thêm ở bước sau
        int index = -1;
        //TODO TODO TODO TODO TODO bước 1
        for(auto & i : areaTable) if (i->ID == ID) {index = ID; break;}


        //*bước 2: xem thử có khu này trong heap chưa để thêm vô
        if(index == -1){ // Not Exist
            areaTable.push_back(new Node(ID));
            index = areaTable.size() - 1;
            areaTable[index]->insert(result);
            this->moveTop(areaTable[index]);
            this->ReHeap_up(index);
        }
            //*bước 3: thêm khách hàng mới vào khu khách hàng muốn thêm vào và tiến hàn reheap down bàn này xuống vì có số khách đông hơn
        else
        {
            areaTable[index]->insert(result);
            this->moveTop(areaTable[index]);
            this->ReHeap_down(index);
        }
    }

    void remove_KEITEIKEN(int number)
    {
        if(areaTable.size() <= 0) return;

        //* đuổi num khách hàng tại num khu vực
        int numberRemove = number;
        while(areaTable.size() != 0 && number != 0){
            //* lấy ra khu đang ở đầu đầu heap xóa number khách hàng đầu linklist
            cout << "remove customers in the area ID = " << areaTable[0]->ID << ": ";
            areaTable[0]->remove(numberRemove);
            cout << "\n";

            //* trường hợp xóa hết thì xóa nó trong heap sau đó reheap down khu xuống vì đang ở đầu hàng
            if(areaTable[0]->size() == 0)
            {
                swap(areaTable[0], areaTable[areaTable.size() - 1]);
                //! xóa nó khỏi danh sách liên kết
                this->removeNode(areaTable[areaTable.size() - 1]);
                delete areaTable[areaTable.size() - 1];

                //! xóa trong heap nữa
                areaTable.pop_back();
            }
            this->ReHeap_down(0);
            number --;
        }
    }
//^hàm in ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void print_pre_order(int index, int number)
    {
        if(index >= this->areaTable.size()) return;

        this->areaTable[index]->print(number);
        print_pre_order(index * 2 + 1, number);
        print_pre_order(index * 2 + 2, number);
    }
    void print_CLEAVE(int number)
    {
        if(number <= 0) return;

        cout << "Heap : ";
        for(auto it : this->areaTable)
        {
            int order = 0;
            for (auto ix : LRU) {
                if (ix == it) break;
                ++order;
            }
            cout << it->ID << "(len="<< it->size() <<",index=" << order <<")" << " ";
        }
        cout << "\n";

        cout << "Heap : ";
        for(auto it : this->areaTable) cout << it->ID  << " ";
        cout << "\n";

        cout << "list LRU : ";
        for(auto it : LRU) cout << it->ID << " ";
        cout << "\n";


        print_pre_order(0, number);
    }
//^ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
    class Node{
    private:
        int ID;					//! ID của bàn đó
        list<int> head; 		//! lưu danh sách các result của khách hàng
        friend class RESTAURANT_Sukuna;
    public:
        Node(int ID) : ID(ID) {}
        int size() const { return head.size(); }
        //* thêm vô đầu danh sách
        void insert(int result){head.push_front(result);}
        //* xóa ở cuối với số lượng là number cơ chế FIFO vô sớm thì cút sớm
        void remove(int number)
        {
            //TODO: xóa number khác hàng ở cuối danh sách tương ứng với vô sớm nhất
            //^ gợi ý dùng hàm của linklist có sẵn
            if (number == 0) return;
            while (number) {
                if (head.size() == 0) return;
                head.pop_front();
                number--;
            }
            //* thêm cout << head.back() << " "; để in ra
        }
        //* print ra number khách hàng mới đến gần nhất theo cơ chế LIFO các khách hàng gần nhất
        void print(int number)
        {
            cout << "customers in the area ID = " << ID << ": ";
            for(list<int>::iterator it = head.begin(); number > 0 && it != head.end(); ++it, --number)
            {
                cout << *it << " ";
            }
            cout << "\n";
        }

    };

};

RESTAURANT_Gojo* gojo = nullptr;
RESTAURANT_Sukuna* sukuna = nullptr;
void simulate(const string &filename)
{
    string item;
    ifstream  myfile(filename);
    if (myfile.is_open()) {
        while (myfile >> item) {
            if (item == "MAXSIZE") {
                myfile>>item;
                MAXSIZE = stoi(item);
                cout << "MAXSIZE: " << MAXSIZE << endl;
            }
            else if (item == "LAPSE") {
                myfile>>item;
                LAPSE(item);
            }
            else if (item == "KOKUSEN") {
                KOKUSEN();
            }
            else if (item == "KEITEIKEN") {
                myfile>>item;
                KEITEIKEN(stoi(item));
            }
            else if (item == "HAND") {
                HAND();
            }
            else if (item == "LIMITLESS") {
                myfile>>item;
                LIMITLESS(stoi(item));
            }
            else if (item == "CLEAVE") {
                myfile>>item;
                CLEAVE(stoi(item));
            }
        }
    }
    else cout << "Can not open file\n";
    cout << "Good Luck";
}
// ==================LAPSE CODE======================

bool cmp_1 (pair<char, Data>& a, pair<char, Data>& b) {
    if (a.second.freq > b.second.freq) return false;
    else if (a.second.freq == b.second.freq) return a.second.timestamp < b.second.timestamp;
    else return true;
}
char CAESAR(char x, int num) {
    num = num % 26;
    int star_a = 65;
    int star_A = 97;
    int num_alphabet = 26;
    if (isupper(x))return char(int(x + num - star_a) % num_alphabet + star_a);
        // Encrypt Lowercase letters
    else return char(int(x + num - star_A) % num_alphabet + star_A);
}

int binaryToDecimal(int n) {
    int num = n;
    int dec_value = 0;
    // Initializing base value to
    // 1, i.e 2^0
    int base = 1;
    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;
        dec_value += last_digit * base;
        base = base * 2;
    }
    return dec_value;
}
void LAPSE(const string & name) {
    /** 1. STANDARDIZED NAME*/
    cout << "Call LAPSE\nName: " << name << "\n";
    map<char, Data> mp;
    for (size_t i = 0, n = name.size(); i < n; i++) {
        char c = name[i];
        if(mp.find(c) == mp.end()) {
            mp[c].freq = 1;
            mp[c].timestamp = (int)i;
        }
        else mp[c].freq++;
    }
    string newName;
    for (char i : name) {
        cout << "char " << i << " freq: " << mp[i].freq << endl;
        char c =  CAESAR(i, mp[i].freq);
        newName+=c;
    }
    cout << "New_Name: " << newName << "\n";
    // convert MAP to VECTOR for CAESAR encoding
    vector<pair<char, Data>> vec;
    vec.reserve(mp.size());
    for (auto i : mp) vec.emplace_back(i);
    if (vec.size() < 3) return;
    for (auto i : vec) {
        cout << "Char: " << i.first << endl;
        cout << "Fre: " << i.second.freq << endl;
    }
    cout << "AFTER CAESAR\n";
    mp.clear();
    for (auto i : vec) {
        i.first = CAESAR(i.first, i.second.freq);
        cout << "Char: " << i.first << endl;
        cout << "Fre: " << i.second.freq << endl;
        char c = i.first;
        if (mp.find(c) == mp.end()) {
            mp[c].freq = i.second.freq;
            mp[c].timestamp = 0; // reset value for SORTING
        }
            // Accumulate the same values
        else mp[c].freq++;
    }
    vector<pair<char, Data>> listX;
    listX.reserve(mp.size());
    for (auto i : mp)  listX.emplace_back(i);
    sort(listX.begin(), listX.end(), cmp_1);
    cout << "AFTER ACCUMULATE AND SORT\n";
    cout << "LIST X IS: \n";
    for (auto i : listX) {
        cout << "Char: " << i.first << endl;
        cout << "Fre: " << i.second.freq << endl;
    }
    auto tree = make_unique<HuffTree>();
    vector<unique_ptr<HuffTree>> ls;
    for (int i = 0; i < listX.size(); i++) {
        auto temp = make_unique<HuffTree>(make_unique<HuffNode>(listX[i].second.freq, listX[i].first));
        ls.push_back(std::move(temp));
    }
    sort(ls.begin(), ls.end(), cmp);
    for (int i = 0; i < ls.size(); i++ ) cout << "List" << i << " node: " << ls[i]->get_c() << " freq: " << ls[i]->get_freq() << endl;
    while (ls.size() > 1) {
        cout << "InLOOP\n";
        auto tmp1 = std::move(ls.front());
        ls.erase(ls.begin());
        auto tmp2 = std::move(ls.front());
        ls.erase(ls.begin());
        auto tmp3 = make_unique<HuffNode>(tmp1->get_freq() + tmp2->get_freq(), '\0', tmp1->get_root(), tmp2->get_root());
        auto temp_tree = make_unique<HuffTree>(std::move(tmp3));
        temp_tree->get_unbalance();
        temp_tree->get_unbalance();
        temp_tree->get_unbalance();
        ls.insert(ls.begin(), std::move(temp_tree));
        sort(ls.begin(), ls.end(), cmp);

    }
    tree = std::move(ls[0]);
    // the first node of tree is a tree (rotated)
    cout << "PreOrder\n";
    tree->preorderTraversal([](char c, int freq, int height) {
        cout << "Node: " << c << " with frequency: " << freq  << " and height: " << height << endl;}
    );

    string bin = "";
    for (unsigned  long n = newName.size(), i = 10; i > 0; --i) {
        string bi = tree->to_bin(newName[i]);
        bin = bi + bin;
        if (bin.size() >= 10) {
            bin.substr(bin.size() - 10, 10);
            break;
        }
    }

    cout << bin << endl;
    int Result = binaryToDecimal(stoi(bin));
    cout << Result << endl;

    /** 2. CHOSE RESTAURANT*/
    if (Result % 2 != 0) gojo->insertAreaTable(Result);
    else sukuna->insertAreaTable(Result);

    /** 3. CHOSE RESTAURANT*/
    cout << "End LAPSE\n";
}
// ==================END LAPSE CODE==================
void KOKUSEN(){
    cout << "Call KEITEIKEN\n";
    cout << "End KEITEIKEN\n";
}
void KEITEIKEN(int num) {
    cout << "Call KEITEIKEN\nNum: " << num << "\n";
    cout << "Result-ID\n";
    cout << "End KEITEIKEN\n";
}
void HAND() {}
void LIMITLESS(int num) {
    cout << "Call LIMITLESS\nNum: " << num << "\n";
    cout << "End LIMITLESS\n";
}
void CLEAVE(int num) {
    cout << "Call CLEAVE\nNum: " << num << "\n";
    cout << "End CLEAVE\n";
}

