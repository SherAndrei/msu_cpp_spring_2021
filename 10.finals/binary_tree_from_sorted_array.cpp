#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>
#include <numeric>

/*
Дан отсортированный вектор, построить бинарное дерево максимально сбалансированное

TreeNode* sortedArrayToBST(vector<int>& nums) {

}
*/

struct TreeNode {
    int v;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

TreeNode* sortedArrayToBST(std::vector<int>& nums) {
    if (nums.empty()) return nullptr;

    if (nums.size() == 1) {
        TreeNode* root = new TreeNode{nums.front()};
        return root;
    }

    if (!std::is_sorted(nums.begin(), nums.end())) {
        throw std::runtime_error("");
    }

    int mid = nums.size() / 2;
    TreeNode* root = new TreeNode{nums[mid]};

    std::vector<int> left(nums.begin(), nums.begin() + mid);
    std::vector<int> right(nums.begin() + mid + 1, nums.end());

    root->left = sortedArrayToBST(left);
    root->right = sortedArrayToBST(right);

    return root;
}

bool isLeaf(TreeNode* node) {
    return node->left == nullptr && node->right == nullptr;
}

void testNotSorted() {
    std::vector<int> vec = {1, 4, 2};
    try {
        sortedArrayToBST(vec);
        assert(false);
    } catch (std::exception& ex) {
        assert(true);
    }
    std::cerr << "TestNotSorted OK" << std::endl;
}

void TestOne() {
    std::vector<int> vec = {1};
    TreeNode* head = sortedArrayToBST(vec);
    assert(head->v == 1);
    assert(isLeaf(head));
    std::cerr << "TestOne OK" << std::endl;
}

void TestSmall() {
    {
        std::vector<int> vec = {1, 2, 3};
        TreeNode* head = sortedArrayToBST(vec);
        assert(head->v == 2);

        assert(head->left->v == 1);
        assert(isLeaf(head->left));

        assert(head->right->v == 3);
        assert(isLeaf(head->right));
    }
    {
        std::vector<int> vec = {-3, -2, -1};
        TreeNode* head = sortedArrayToBST(vec);
        assert(head->v == -2);

        assert(head->left->v == -3);
        assert(isLeaf(head->left));

        assert(head->right->v == -1);
        assert(isLeaf(head->right));
    }
    std::cerr << "TestSmall OK" << std::endl;
}

TreeNode* findLeftLeaf(TreeNode* node) {
    if (node->left == nullptr)
        return node;

    return findLeftLeaf(node->left);
}

TreeNode* findRightLeaf(TreeNode* node) {
    if (node->right == nullptr)
        return node;

    return findRightLeaf(node->right);
}

void TestBig() {
    std::vector<int> vec(100);
    std::iota(vec.begin(), vec.end(), 0);

    TreeNode* head = sortedArrayToBST(vec);
    assert(head->v == 50);
    assert(findLeftLeaf(head)->v == 0);
    assert(findRightLeaf(head)->v == 99);

    std::cerr << "TestBig OK" << std::endl;
}

void TestEqual() {
    std::vector<int> vec = {8, 8, 8};
    TreeNode* head = sortedArrayToBST(vec);
    assert(head->v == 8);

    assert(head->left->v == 8);
    assert(isLeaf(head->left));

    assert(head->right->v == 8);
    assert(isLeaf(head->right));

    std::cerr << "TestEqual OK" << std::endl;
}

void TestMinus() {
    std::vector<int> vec = {-8, 8, 8};
    TreeNode* head = sortedArrayToBST(vec);
    assert(head->v == 8);

    assert(head->left->v == -8);
    assert(isLeaf(head->left));

    assert(head->right->v == 8);
    assert(isLeaf(head->right));
    std::cerr << "TestMinus OK" << std::endl;
}

int main() {
    TestOne();
    testNotSorted();
    TestSmall();
    TestEqual();
    TestMinus();
    TestBig();
}
