#include <bits/stdc++.h>
using namespace std;

class Node
{
public:
    string tagName;
    unordered_map<string, string> attributes;
    string content;
    Node *parent;
    vector<unique_ptr<Node>> children;
    Node *nextSibling;
    Node *prevSibling;

    Node(const string &tag) : tagName(tag), parent(nullptr), nextSibling(nullptr), prevSibling(nullptr) {}
};

class HTMLParser
{
private:
    unique_ptr<Node> root;
    vector<string> tokens;

    void tokenize(const string &html)
    {
        regex tokenRegex("<[^>]+>|[^<]+");
        auto begin = sregex_iterator(html.begin(), html.end(), tokenRegex);
        auto end = sregex_iterator();

        for (sregex_iterator i = begin; i != end; ++i)
        {
            tokens.push_back(i->str());
        }
    }

    void buildTree()
    {
        stack<Node *> nodeStack;
        root = make_unique<Node>("root");
        Node *currentNode = root.get();
        nodeStack.push(currentNode);

        for (const auto &token : tokens)
        {
            if (token[0] == '<' && token[1] != '/')
            {
                // Start tag or self-closing tag
                string tagName = token.substr(1, token.find_first_of(" >") - 1);
                auto newNode = make_unique<Node>(tagName);
                newNode->parent = currentNode;

                // Parse attributes
                regex attrRegex("(\\w+)=['\"](.*?)['\"]");
                auto attrBegin = sregex_iterator(token.begin(), token.end(), attrRegex);
                auto attrEnd = sregex_iterator();

                for (sregex_iterator i = attrBegin; i != attrEnd; ++i)
                {
                    smatch match = *i;
                    newNode->attributes[match[1]] = match[2];
                }

                if (token.back() != '/')
                {
                    // Regular start tag
                    if (!currentNode->children.empty())
                    {
                        newNode->prevSibling = currentNode->children.back().get();
                        currentNode->children.back()->nextSibling = newNode.get();
                    }
                    currentNode->children.push_back(move(newNode));
                    currentNode = currentNode->children.back().get();
                    nodeStack.push(currentNode);
                }
                else
                {
                    // Self-closing tag
                    if (!currentNode->children.empty())
                    {
                        newNode->prevSibling = currentNode->children.back().get();
                        currentNode->children.back()->nextSibling = newNode.get();
                    }
                    currentNode->children.push_back(move(newNode));
                }
            }
            else if (token[0] == '<' && token[1] == '/')
            {
                // End tag
                nodeStack.pop();
                if (!nodeStack.empty())
                {
                    currentNode = nodeStack.top();
                }
            }
            else
            {
                // Text content
                currentNode->content += token;
            }
        }
    }

    void printDFS(Node *node, int depth) const
    {
        if (!node)
            return;

        // Print node details
        if (depth > 0)
        {
            cout << "  Children of: " << node->tagName << endl;
        }
        cout << "  Node: " << node->tagName << endl;

        if (!node->attributes.empty())
        {
            cout << "    Attributes: ";
            for (const auto &attr : node->attributes)
            {
                cout << attr.first << "=\"" << attr.second << "\" ";
            }
            cout << endl;
        }

        if (!node->content.empty())
        {
            cout << "    Content: " << node->content << endl;
        }

        if (!node->children.empty())
        {
            for (const auto &child : node->children)
            {
                printDFS(child.get(), depth + 1);
            }
        }
    }

public:
    void parse(const string &html)
    {
        tokenize(html);
        buildTree();
    }

    void printDFS() const
    {
        if (root)
        {
            printDFS(root.get(), 0);
        }
    }

    void listChildren(Node *node) const
    {
        if (!node)
            return;

        cout << "Children of " << node->tagName << ":" << endl;
        for (const auto &child : node->children)
        {
            cout << "  " << child->tagName << endl;
        }
    }

    void printSiblings(Node *node) const
    {
        if (!node)
            return;

        if (node->prevSibling)
        {
            cout << "Previous sibling of " << node->tagName << ": " << node->prevSibling->tagName << endl;
        }
        else
        {
            cout << node->tagName << " has no previous sibling." << endl;
        }

        if (node->nextSibling)
        {
            cout << "Next sibling of " << node->tagName << ": " << node->nextSibling->tagName << endl;
        }
        else
        {
            cout << node->tagName << " has no next sibling." << endl;
        }
    }

    vector<Node *> findByTagName(const string &tagName) const
    {
        vector<Node *> results;

        function<void(Node *)> dfs = [&](Node *node)
        {
            if (!node)
                return;

            if (node->tagName == tagName)
            {
                results.push_back(node);
            }

            for (const auto &child : node->children)
            {
                dfs(child.get());
            }
        };

        dfs(root.get());
        return results;
    }
};

int main()
{
    HTMLParser parser;
    string html = R"(
        <html>
            <head>
                <title>My Page</title>
            </head>
            <body>
                <h1>Welcome</h1>
                <p>This is a <strong>test</strong> paragraph.</p>
                <img src="image.png" alt="Test Image"/>
            </body>
        </html>
    )";

    parser.parse(html);
    parser.printDFS();

    Node *bodyNode = parser.findByTagName("body").front();
    parser.listChildren(bodyNode);
    Node *pNode = parser.findByTagName("h1").front();
    parser.printSiblings(pNode);

    return 0;
}
