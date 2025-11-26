/*
 * ITERATOR PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Provides a way to access elements of an aggregate object sequentially
 * without exposing its underlying representation.
 * 
 * WHEN TO USE:
 * - Need to access contents of aggregate without exposing internal structure
 * - Need to support multiple traversals of aggregate objects
 * - Want uniform interface for traversing different structures
 * 
 * PROS:
 * + Simplifies aggregate interface
 * + Multiple iterators and traversals can be active
 * + Single Responsibility: separates traversal from collection
 * + Open/Closed: can add new iterators without changing collection
 * 
 * CONS:
 * - Overkill for simple collections
 * - Can be less efficient than direct access
 * 
 * REAL-WORLD EXAMPLES:
 * - STL iterators (C++)
 * - Java Collections
 * - Database cursors
 * - File system traversal
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// ============= Example 1: Book Collection =============

class Book {
private:
    string title;
    string author;
    
public:
    Book(const string& t, const string& a) : title(t), author(a) {}
    
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    
    void display() const {
        cout << "\"" << title << "\" by " << author << endl;
    }
};

// Iterator interface
template<typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual bool hasNext() const = 0;
    virtual T next() = 0;
    virtual void reset() = 0;
};

// Aggregate interface
template<typename T>
class Collection {
public:
    virtual ~Collection() = default;
    virtual shared_ptr<Iterator<T>> createIterator() = 0;
};

// Concrete Collection
class BookCollection : public Collection<Book> {
private:
    vector<Book> books;
    
public:
    void addBook(const Book& book) {
        books.push_back(book);
    }
    
    int size() const { return books.size(); }
    
    Book& getAt(int index) { return books[index]; }
    
    // Factory method for iterator
    shared_ptr<Iterator<Book>> createIterator() override;
    
    friend class BookIterator;
};

// Concrete Iterator
class BookIterator : public Iterator<Book> {
private:
    BookCollection& collection;
    int currentIndex;
    
public:
    BookIterator(BookCollection& coll) : collection(coll), currentIndex(0) {}
    
    bool hasNext() const override {
        return currentIndex < collection.size();
    }
    
    Book next() override {
        return collection.getAt(currentIndex++);
    }
    
    void reset() override {
        currentIndex = 0;
    }
};

shared_ptr<Iterator<Book>> BookCollection::createIterator() {
    return make_shared<BookIterator>(*this);
}

// ============= Example 2: Tree Traversal =============

template<typename T>
class TreeNode {
public:
    T data;
    shared_ptr<TreeNode<T>> left;
    shared_ptr<TreeNode<T>> right;
    
    TreeNode(T val) : data(val), left(nullptr), right(nullptr) {}
};

template<typename T>
class BinaryTree {
private:
    shared_ptr<TreeNode<T>> root;
    
public:
    BinaryTree() : root(nullptr) {}
    
    void setRoot(shared_ptr<TreeNode<T>> node) { root = node; }
    shared_ptr<TreeNode<T>> getRoot() const { return root; }
    
    // Different iterator types
    shared_ptr<Iterator<T>> createInOrderIterator();
    shared_ptr<Iterator<T>> createPreOrderIterator();
};

// InOrder Iterator (Left -> Root -> Right)
template<typename T>
class InOrderIterator : public Iterator<T> {
private:
    vector<T> elements;
    int currentIndex;
    
    void inOrderTraversal(shared_ptr<TreeNode<T>> node) {
        if (node) {
            inOrderTraversal(node->left);
            elements.push_back(node->data);
            inOrderTraversal(node->right);
        }
    }
    
public:
    InOrderIterator(shared_ptr<TreeNode<T>> root) : currentIndex(0) {
        inOrderTraversal(root);
    }
    
    bool hasNext() const override {
        return currentIndex < elements.size();
    }
    
    T next() override {
        return elements[currentIndex++];
    }
    
    void reset() override {
        currentIndex = 0;
    }
};

// PreOrder Iterator (Root -> Left -> Right)
template<typename T>
class PreOrderIterator : public Iterator<T> {
private:
    vector<T> elements;
    int currentIndex;
    
    void preOrderTraversal(shared_ptr<TreeNode<T>> node) {
        if (node) {
            elements.push_back(node->data);
            preOrderTraversal(node->left);
            preOrderTraversal(node->right);
        }
    }
    
public:
    PreOrderIterator(shared_ptr<TreeNode<T>> root) : currentIndex(0) {
        preOrderTraversal(root);
    }
    
    bool hasNext() const override {
        return currentIndex < elements.size();
    }
    
    T next() override {
        return elements[currentIndex++];
    }
    
    void reset() override {
        currentIndex = 0;
    }
};

template<typename T>
shared_ptr<Iterator<T>> BinaryTree<T>::createInOrderIterator() {
    return make_shared<InOrderIterator<T>>(root);
}

template<typename T>
shared_ptr<Iterator<T>> BinaryTree<T>::createPreOrderIterator() {
    return make_shared<PreOrderIterator<T>>(root);
}

// ============= Example 3: Social Media Feed =============

class Post {
private:
    string author;
    string content;
    int likes;
    
public:
    Post(const string& a, const string& c, int l = 0) 
        : author(a), content(c), likes(l) {}
    
    void display() const {
        cout << author << ": " << content << " [" << likes << " likes]" << endl;
    }
    
    int getLikes() const { return likes; }
};

class SocialMediaFeed : public Collection<Post> {
private:
    vector<Post> posts;
    
public:
    void addPost(const Post& post) {
        posts.push_back(post);
    }
    
    int size() const { return posts.size(); }
    Post& getAt(int index) { return posts[index]; }
    
    shared_ptr<Iterator<Post>> createIterator() override;
    shared_ptr<Iterator<Post>> createReverseIterator();
    shared_ptr<Iterator<Post>> createPopularPostsIterator(int minLikes);
    
    friend class FeedIterator;
    friend class ReverseFeedIterator;
    friend class PopularPostsIterator;
};

// Forward Iterator
class FeedIterator : public Iterator<Post> {
private:
    SocialMediaFeed& feed;
    int currentIndex;
    
public:
    FeedIterator(SocialMediaFeed& f) : feed(f), currentIndex(0) {}
    
    bool hasNext() const override {
        return currentIndex < feed.size();
    }
    
    Post next() override {
        return feed.getAt(currentIndex++);
    }
    
    void reset() override {
        currentIndex = 0;
    }
};

// Reverse Iterator
class ReverseFeedIterator : public Iterator<Post> {
private:
    SocialMediaFeed& feed;
    int currentIndex;
    
public:
    ReverseFeedIterator(SocialMediaFeed& f) : feed(f) {
        currentIndex = feed.size() - 1;
    }
    
    bool hasNext() const override {
        return currentIndex >= 0;
    }
    
    Post next() override {
        return feed.getAt(currentIndex--);
    }
    
    void reset() override {
        currentIndex = feed.size() - 1;
    }
};

// Filtered Iterator (only popular posts)
class PopularPostsIterator : public Iterator<Post> {
private:
    SocialMediaFeed& feed;
    int minLikes;
    int currentIndex;
    
    void findNext() {
        while (currentIndex < feed.size() && 
               feed.getAt(currentIndex).getLikes() < minLikes) {
            currentIndex++;
        }
    }
    
public:
    PopularPostsIterator(SocialMediaFeed& f, int likes) 
        : feed(f), minLikes(likes), currentIndex(0) {
        findNext();
    }
    
    bool hasNext() const override {
        return currentIndex < feed.size();
    }
    
    Post next() override {
        Post post = feed.getAt(currentIndex++);
        findNext();
        return post;
    }
    
    void reset() override {
        currentIndex = 0;
        findNext();
    }
};

shared_ptr<Iterator<Post>> SocialMediaFeed::createIterator() {
    return make_shared<FeedIterator>(*this);
}

shared_ptr<Iterator<Post>> SocialMediaFeed::createReverseIterator() {
    return make_shared<ReverseFeedIterator>(*this);
}

shared_ptr<Iterator<Post>> SocialMediaFeed::createPopularPostsIterator(int minLikes) {
    return make_shared<PopularPostsIterator>(*this, minLikes);
}

int main() {
    cout << "=== ITERATOR PATTERN DEMO ===" << endl;
    
    // Example 1: Book Collection
    cout << "\n1. BOOK COLLECTION:" << endl;
    cout << "===================" << endl;
    
    BookCollection library;
    library.addBook(Book("1984", "George Orwell"));
    library.addBook(Book("To Kill a Mockingbird", "Harper Lee"));
    library.addBook(Book("The Great Gatsby", "F. Scott Fitzgerald"));
    library.addBook(Book("Pride and Prejudice", "Jane Austen"));
    
    cout << "\n[Iterating through books]" << endl;
    auto bookIterator = library.createIterator();
    while (bookIterator->hasNext()) {
        Book book = bookIterator->next();
        book.display();
    }
    
    // Example 2: Tree Traversal
    cout << "\n\n2. BINARY TREE TRAVERSAL:" << endl;
    cout << "=========================" << endl;
    
    BinaryTree<int> tree;
    auto root = make_shared<TreeNode<int>>(5);
    root->left = make_shared<TreeNode<int>>(3);
    root->right = make_shared<TreeNode<int>>(8);
    root->left->left = make_shared<TreeNode<int>>(1);
    root->left->right = make_shared<TreeNode<int>>(4);
    root->right->left = make_shared<TreeNode<int>>(7);
    root->right->right = make_shared<TreeNode<int>>(9);
    tree.setRoot(root);
    
    cout << "\n[In-Order Traversal]" << endl;
    auto inOrder = tree.createInOrderIterator();
    while (inOrder->hasNext()) {
        cout << inOrder->next() << " ";
    }
    cout << endl;
    
    cout << "\n[Pre-Order Traversal]" << endl;
    auto preOrder = tree.createPreOrderIterator();
    while (preOrder->hasNext()) {
        cout << preOrder->next() << " ";
    }
    cout << endl;
    
    // Example 3: Social Media Feed
    cout << "\n\n3. SOCIAL MEDIA FEED:" << endl;
    cout << "=====================" << endl;
    
    SocialMediaFeed feed;
    feed.addPost(Post("Alice", "Hello World!", 5));
    feed.addPost(Post("Bob", "Learning design patterns", 50));
    feed.addPost(Post("Charlie", "C++ is awesome", 120));
    feed.addPost(Post("Diana", "Good morning!", 8));
    feed.addPost(Post("Eve", "Iterator pattern explained", 200));
    
    cout << "\n[Forward iteration]" << endl;
    auto forwardIter = feed.createIterator();
    while (forwardIter->hasNext()) {
        forwardIter->next().display();
    }
    
    cout << "\n[Reverse iteration]" << endl;
    auto reverseIter = feed.createReverseIterator();
    while (reverseIter->hasNext()) {
        reverseIter->next().display();
    }
    
    cout << "\n[Popular posts only (>= 50 likes)]" << endl;
    auto popularIter = feed.createPopularPostsIterator(50);
    while (popularIter->hasNext()) {
        popularIter->next().display();
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Iterator provides SEQUENTIAL ACCESS to collection elements" << endl;
    cout << "2. Hides internal structure of collection" << endl;
    cout << "3. Can have multiple iterators active simultaneously" << endl;
    cout << "4. Supports different traversal strategies (forward, reverse, filtered)" << endl;
    cout << "5. Separates traversal logic from collection logic" << endl;
    cout << "6. Foundation of C++ STL and many modern collection libraries" << endl;
    
    return 0;
}

